#include "common/utilsMat.h"
#include "common/utils.h"
#include "common/constants.h"
#include "components/Transform.h"
#include "Components/Force.h"
#include <cmath>

namespace myutils
{
    void ApplyTranslation(glm::vec3 position, glm::mat4& matModelOut)
    {
        // Translation
        glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f), position);

        matModelOut = matModelOut * matTranslate;
    }

    void ApplyRotation(glm::quat qOrientation, glm::mat4& matModelOut)
    {
        // Rotation matrix generation
        glm::mat4 matRotation = glm::mat4(qOrientation);

        matModelOut = matModelOut * matRotation;
    }

    void ApplyScale(float scale, glm::mat4& matModelOut)
    {
        // Scaling matrix
        glm::mat4 matScale = glm::scale(glm::mat4(1.0f), glm::vec3(scale));

        matModelOut = matModelOut * matScale;
    }

    void ApplyTransformInModelMat(iComponent* pTransform, glm::mat4& matModelOut)
    {
        TransformComponent* pTransComp = dynamic_cast<TransformComponent*>(pTransform);
        if (!pTransComp)
        {
            CheckEngineError("Wrong component type!");
            return;
        }

        // Combine all the transformations
        ApplyTranslation(pTransComp->GetPosition(), matModelOut);
        ApplyRotation(pTransComp->GetQuatOrientation(), matModelOut);
        ApplyScale(pTransComp->GetScale(), matModelOut);

        return;
    }

    void ApplyTransformInModelMat(glm::vec3 position, glm::vec3 quaternions, float scale, glm::mat4& matModelOut)
    {
        // Combine all the transformations
        ApplyTranslation(position, matModelOut);
        ApplyRotation(quaternions, matModelOut);
        ApplyScale(scale, matModelOut);

        return;
    }

    double distance(const glm::vec3& p1, const glm::vec3& p2)
    {
        return std::hypot(p1.x - p2.x, p1.y - p2.y);
    }

    // Function to determine if a point is inside the circumcircle of a triangle
    bool IsInsideCircumcircle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& testPoint)
    {
        // Calculate the distances between the three points that form the triangle
        double d12 = distance(p1, p2);
        double d23 = distance(p2, p3);
        double d31 = distance(p3, p1);

        // Calculate the determinant of a matrix formed by the triangle's coordinates
        double determinant = p1.x * (p2.y - p3.y) - p2.x * (p1.y - p3.y) + p3.x * (p1.y - p2.y);

        // Calculate the radius of the circumcircle
        double radius = (d12 * d23 * d31) / (4.0 * std::abs(determinant));

        // Calculate the distance between the test point and the first point of the triangle
        double distToTestPoint = distance(testPoint, p1);

        // Check if the distance to the test point is less than or equal to the circumcircle radius
        return distToTestPoint <= radius;
    }

    // From: Real-Time Collision Detection- Ericson, Christer- 9781558607323- Books - Amazon.ca
    // Chapter 5:
    glm::vec3 ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
    {
        glm::vec3 ab = b - a;
        glm::vec3 ac = c - a;
        glm::vec3 bc = c - b;

        // Compute parametric position s for projection P' of P on AB,
        // P' = A + s*AB, s = snom/(snom+sdenom)
        float snom = glm::dot(p - a, ab), sdenom = glm::dot(p - b, a - b);

        // Compute parametric position t for projection P' of P on AC,
        // P' = A + t*AC, s = tnom/(tnom+tdenom)
        float tnom = glm::dot(p - a, ac), tdenom = glm::dot(p - c, a - c);

        if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out

        // Compute parametric position u for projection P' of P on BC,
        // P' = B + u*BC, u = unom/(unom+udenom)
        float unom = glm::dot(p - b, bc), udenom = glm::dot(p - c, b - c);

        if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
        if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out


        // P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
        glm::vec3 n = glm::cross(b - a, c - a);
        float vc = glm::dot(n, glm::cross(a - p, b - p));
        // If P outside AB and within feature region of AB,
        // return projection of P onto AB
        if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
            return a + snom / (snom + sdenom) * ab;

        // P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
        float va = glm::dot(n, glm::cross(b - p, c - p));
        // If P outside BC and within feature region of BC,
        // return projection of P onto BC
        if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
            return b + unom / (unom + udenom) * bc;

        // P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
        float vb = glm::dot(n, glm::cross(c - p, a - p));
        // If P outside CA and within feature region of CA,
        // return projection of P onto CA
        if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
            return a + tnom / (tnom + tdenom) * ac;

        // P must project inside face region. Compute Q using barycentric coordinates
        float u = va / (va + vb + vc);
        float v = vb / (va + vb + vc);
        float w = 1.0f - u - v; // = vc / (va + vb + vc)
        return u * a + v * b + w * c;
    }

    // From: Real-Time Collision Detection- Ericson, Christer- 9781558607323- Books - Amazon.ca
    // Chapter 5:
    glm::vec3 ClosestPtPointOBB(glm::vec3 p, glm::vec3 obbCenter, glm::mat3 obbRotation, glm::vec3 maxXYZ)
    {
        glm::vec3 d = p - obbCenter;
        // Start result at center of box; make steps from there
        glm::vec3 q = obbCenter;
        // For each OBB axis...
        for (int i = 0; i < 3; i++) {
            // ...project d onto that axis to get the distance
            // along the axis of d from the box center
            float dist = glm::dot(d, obbRotation[i]);
            // If distance farther than the box extents, clamp to the box
            if (dist > maxXYZ[i]) dist = maxXYZ[i];
            if (dist < -maxXYZ[i]) dist = -maxXYZ[i];
            // Step that distance along the axis to get world coordinate
            q += dist * obbRotation[i];
        }

        return q;
    }

    glm::vec3 GetCollisionNormal(glm::vec3 velocityA, glm::vec3 velocityB)
    {
        glm::vec3 normal = glm::normalize(velocityA - velocityB);
        return normal;
    }

    glm::vec3 GetNormal(glm::vec3 collisionPoint, glm::vec3 center)
    {
        glm::vec3 normal = glm::normalize(collisionPoint - center);
        return normal;
    }

    glm::vec3 GetNormal(glm::vec3 vertices[3])
    {
        // Get triangle hit normals
        glm::vec3 edgeA = vertices[1] - vertices[0];
        glm::vec3 edgeB = vertices[2] - vertices[0];
        glm::vec3 triNormal = glm::normalize(glm::cross(edgeA, edgeB));

        return triNormal;
    }

    glm::vec3 GetReflectionNormal(glm::vec3 direction, glm::vec3 triangleVertices[3])
    {
        // Normalize sphere direction
        glm::vec3 sphereDirection = direction;
        sphereDirection = glm::normalize(sphereDirection);

        // Get triangle hit normals
        glm::vec3 triNormal = myutils::GetNormal(triangleVertices);

        // Calculate the reflection vector from the normal	
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/reflect.xhtml
        // 1st parameter is the "incident" vector
        // 2nd parameter is the "normal" vector
        glm::vec3 reflectionNormal = glm::reflect(sphereDirection, triNormal);

        return reflectionNormal;
    }

    glm::vec3 GetReflectionNormal(glm::vec3 collisionPoint, glm::vec3 velocityA, glm::vec3 centerB)
    {
        // Normalize A direction
        glm::vec3 normal = myutils::GetNormal(collisionPoint, centerB);

        glm::vec3 reflectionNormal = glm::normalize(velocityA - (2 * (glm::dot(velocityA, normal)) * normal));

        return reflectionNormal;
    }

    glm::vec2 ReflectVector2D(const glm::vec2& normal, const glm::vec2& velocity)
    {
        // Calculate the dot product of this vector and the wall's normal
        float dotProduct = glm::dot(velocity, normal);

        // Calculate the new velocity vector after reflection
        glm::vec2 newVelocity(0);
        newVelocity.x -= 2 * dotProduct * normal.x;
        newVelocity.y -= 2 * dotProduct * normal.y;

        return newVelocity;
    }

    glm::vec3 GetSpheresContactPont(glm::vec3 centerA, float radA, glm::vec3 centerB, float radB)
    {
        return centerA + ((centerB - centerA) * radA / (radA + radB));
    }

    float CalculateSeparatingVelocity(glm::vec3 velocityA, glm::vec3 velocityB, glm::vec3 contactNormal)
    {
        glm::vec3 relativeVelocity = velocityA;
        if (velocityB != glm::vec3(0)) relativeVelocity -= velocityB;
        return glm::dot(relativeVelocity, contactNormal);
    }

    void ResolveVelocity(glm::vec3& velocityA, glm::vec3& velocityB, glm::vec3 contactNormal, float restitution,
        float inverseMassA, float inverseMassB)
    {
        // Find the velocity in the direction of the contact.
        float separatingVelocity = CalculateSeparatingVelocity(velocityA, velocityB, contactNormal);
        // Check if it needs to be resolved.
        if (separatingVelocity > 0)
        {
            // The contact is either separating, or stationary;
            // no extra impulse is required.
            return;
        }

        // Calculate the new separating velocity.
        float newSepVelocity = -separatingVelocity * restitution;
        float deltaVelocity = newSepVelocity - separatingVelocity;
        // We apply the change in velocity to each object in proportion to
        // their inverse mass (i.e., those with lower inverse mass [higher
        // actual mass] get less change in velocity).
        float totalInverseMass = inverseMassA;
        if (velocityB != glm::vec3(0)) totalInverseMass += inverseMassB;
        // If all particles have infinite mass, then impulses have no effect.
        if (totalInverseMass <= 0) return;
        // Calculate the impulse to apply.
        float impulse = deltaVelocity / totalInverseMass;
        // Find the amount of impulse per unit of inverse mass.
        glm::vec3 impulsePerIMass = contactNormal * impulse;
        // Apply impulses: they are applied in the direction of the contact,
        // and are proportional to the inverse mass.
        velocityA = velocityA + impulsePerIMass * inverseMassA;
        if (velocityB != glm::vec3(0))
        {
            // Particle 1 goes in the opposite direction
            velocityB = velocityB + impulsePerIMass * -inverseMassB;
        }

        return;
    }

    void ResolveVelocity(glm::vec3& velocity, glm::vec3 reflectionNormal, float inverseMass)
    {
        float newSpeed = glm::length(velocity) * inverseMass;
        velocity = reflectionNormal * newSpeed;
    }

    float CalculateSinWave(float currentTime, float amplitude, float frequency, float offset) {
        // Calculate the sine value
        float sineValue = (float)(amplitude * sin(2 * PI * frequency * currentTime));

        return offset - sineValue;
    }

    glm::vec3 GetCardinalDirection(glm::vec3 positionA, glm::vec3 positionB)
    {
        using namespace std;
        using namespace glm;

        vec3 direction = positionA - positionB;

        // Possible directions
        const vec3 headings[8] = {
            vec3(1, 0, 0),
            vec3(1, 1, 0),
            vec3(0, 1, 0),
            vec3(-1, 1, 0),
            vec3(-1, 0, 0),
            vec3(-1, -1, 0),
            vec3(0, -1, 0),
            vec3(1, -1, 0)
        };

        // actual conversion code:
        float angle = atan2(direction.y, direction.x);
        int fraction = int(8 * angle / (2 * PI) + 8.5);
        int octant = fraction % 8;

        return headings[octant];
    }

    float GetAngleToPoint(float x, float y)
    {
        // Calculate the angle using atan2 and convert it to degrees
        float angleInRadians = std::atan2(y, x);
        float angleInDegrees = angleInRadians * (180.0f / PI);

        // Ensure the angle is within [0, 360) degrees
        while (angleInDegrees < 0) {
            angleInDegrees += 360.0;
        }

        while (angleInDegrees >= 360) {
            angleInDegrees -= 360.0;
        }

        return angleInDegrees;
    }

    glm::vec3 GetDirectionFromDegrees(const glm::vec3& rotation)
    {
        using namespace glm;

        vec3 rotationRadians = radians(rotation);

        vec3 direction;
        direction.x = cos(rotationRadians.x) * cos(rotationRadians.y);
        direction.y = sin(rotationRadians.y);
        direction.z = sin(rotationRadians.x) * cos(rotationRadians.y);

        vec3 directionNormalized = normalize(direction);

        return directionNormalized;
    }

    std::vector<glm::vec3> GenerateRectangleVertices(const glm::vec3& minXYZ, const glm::vec3& maxXYZ) {
        // Create vertices for a rectangle
        std::vector<glm::vec3> vertices;

        // Define the vertices based on the min and max vectors
        glm::vec3 v0(minXYZ.x, minXYZ.y, minXYZ.z); // bottom-left
        glm::vec3 v1(maxXYZ.x, minXYZ.y, minXYZ.z); // bottom-right
        glm::vec3 v2(maxXYZ.x, maxXYZ.y, minXYZ.z); // top-right
        glm::vec3 v3(minXYZ.x, maxXYZ.y, minXYZ.z); // top-left

        // Add vertices to the vector
        vertices.push_back(v0);
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);

        return vertices;
    }
}