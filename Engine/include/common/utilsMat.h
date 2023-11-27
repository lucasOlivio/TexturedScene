#include "components/iComponent.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace myutils
{
    void ApplyTranslation(glm::vec3 position, glm::mat4& matModelOut);
    void ApplyRotation(glm::quat qOrientation, glm::mat4& matModelOut);
    void ApplyScale(float scale, glm::mat4& matModelOut);

    // TODO: Remove reference  to object here
    void ApplyTransformInModelMat(iComponent* pTransform, glm::mat4& matModelOut);
    void ApplyTransformInModelMat(glm::vec3 position, glm::vec3 quaternions, float scale, glm::mat4& matModelOut);

    double distance(const glm::vec3& p1, const glm::vec3& p2);

    // Function to determine if a point is inside the circumcircle of a triangle
    bool IsInsideCircumcircle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& testPoint);

    // From: Real-Time Collision Detection- Ericson, Christer- 9781558607323- Books - Amazon.ca
    // Chapter 5:
    glm::vec3 ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);

    // From: Real-Time Collision Detection- Ericson, Christer- 9781558607323- Books - Amazon.ca
    // Chapter 5:
    glm::vec3 ClosestPtPointOBB(glm::vec3 p, glm::vec3 obbCenter, glm::mat3 obbRotation, glm::vec3 maxXYZ);

    // Calculates the normal for a sphere based on the collision point and the center of sphere
    glm::vec3 GetNormal(glm::vec3 collisionPoint, glm::vec3 center);

    // Calculates the normal for a triangle based on the vertices positions
    glm::vec3 GetNormal(glm::vec3 vertices[3]);

    // Calculate the reflection normal between and object and a triangle
    glm::vec3 GetReflectionNormal(glm::vec3 direction, glm::vec3 triangleVertices[3]);
    // https://gamedev.stackexchange.com/questions/150322/how-to-find-collision-reflection-vector-on-a-sphere
    glm::vec3 GetReflectionNormal(glm::vec3 collisionPoint, glm::vec3 velocityA, glm::vec3 centerB);

    // Reflect the vector off a normal collision vector
    glm::vec2 ReflectVector2D(const glm::vec2& normal, const glm::vec2& velocity);

    // Check closest contact point between two spheres
    // https://stackoverflow.com/questions/42880053/finding-the-collision-point-of-2-spheres
    glm::vec3 GetSpheresContactPont(glm::vec3 centerA, float radA, glm::vec3 centerB, float radB);

    // Calculate contact normal in object A perspective
    glm::vec3 GetCollisionNormal(glm::vec3 velocityA, glm::vec3 velocityB);

    // Calculates the separating velocity at a contact
    // game-physics-engine-development-how-to-build-a-robust-commercial-grade-physics-engine-for-your-game-second-edition
    // Chapter 7
    float CalculateSeparatingVelocity(glm::vec3 velocityA, glm::vec3 velocityB, glm::vec3 contactNormal);

    // Impulse calculation for the collision with a dynamic object
    // game-physics-engine-development-how-to-build-a-robust-commercial-grade-physics-engine-for-your-game-second-edition
    // Chapter 7
    void ResolveVelocity(glm::vec3& velocityA, glm::vec3& velocityB, glm::vec3 contactNormal, float restitution,
        float inverseMassA, float inverseMassB);

    // Impulse calculation for collision with a static object
    void ResolveVelocity(glm::vec3& velocity, glm::vec3 reflectionNormal, float inverseMass);

    // Calculate sin wave based on time
    float CalculateSinWave(float currentTime, float amplitude, float frequency, float phaseOffset);

    // Given 2 vectors, get the closest cardinal direction from one to another
    // https://gamedev.stackexchange.com/a/49300
    glm::vec3 GetCardinalDirection(glm::vec3 positionA, glm::vec3 positionB);

    // Calculate the angle in degrees to the given point
    float GetAngleToPoint(float x, float y);

    // The direction that this angle represents, assuming on point 0
    glm::vec3 GetDirectionFromDegrees(const glm::vec3& rotation);

    std::vector<glm::vec3> GenerateRectangleVertices(const glm::vec3& minXYZ, const glm::vec3& maxXYZ);
}