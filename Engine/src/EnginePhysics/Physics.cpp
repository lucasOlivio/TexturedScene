#include "EnginePhysics/Physics.h"
#include "common/utilsMat.h"
#include "components/Tag.h"
#include <glm/gtx/string_cast.hpp>

Physics::Physics(SceneView* pSceneView, CollisionEvent* pCollisionEvent)
{
	m_isRunning = false;
	m_pSceneView = pSceneView;
	m_pCollisionEvent = pCollisionEvent;
}

Physics::~Physics()
{

}

void Physics::NewFrame()
{
	for (sCollisionData* pCollision : m_vecCollided)
	{
		delete pCollision;
	}

	m_vecCollided.clear();
	m_vecCollVisited.clear();
}

void Physics::Update(double deltaTime)
{
	if (!m_isRunning)
	{
		return;
	}

	// Change position based on the acceleration and velocity
	for (m_pSceneView->First("force"); !m_pSceneView->IsDone(); m_pSceneView->Next())
	{
		EntityID entityID = m_pSceneView->CurrentKey();
		ForceComponent* pForce = m_pSceneView->CurrentValue<ForceComponent>();
		TransformComponent* pTransform = m_pSceneView->GetComponent<TransformComponent>(entityID, "transform");

		m_ApplyForce(pForce, pTransform, deltaTime);
	}

	// Check if new position is intersecting with other entity
	for (m_pSceneView->First("collision"); !m_pSceneView->IsDone(); m_pSceneView->Next())
	{
		EntityID entityID = m_pSceneView->CurrentKey();
		CollisionComponent* pCollision = m_pSceneView->CurrentValue<CollisionComponent>();
		TransformComponent* pTransform = m_pSceneView->GetComponent<TransformComponent>(entityID, "transform");

		m_CheckCollisions(entityID, pCollision, pTransform);
	}

	// Apply respective response for each collision types
	for (sCollisionData* pCollision : m_vecCollided)
	{
		TransformComponent* pTransformA = m_pSceneView->GetComponent<TransformComponent>(pCollision->entityA, "transform");
		TransformComponent* pTransformB = m_pSceneView->GetComponent<TransformComponent>(pCollision->entityB, "transform");

		// Static bodies won`t have force
		ForceComponent* pForceA = nullptr;
		if (pCollision->bodyTypeA != eBodyType::STATIC)
		{
			ForceComponent* pForceA = m_pSceneView->GetComponent<ForceComponent>(pCollision->entityA, "force");
		}
		ForceComponent* pForceB = nullptr;
		if (pCollision->bodyTypeB != eBodyType::STATIC)
		{
			pForceB = m_pSceneView->GetComponent<ForceComponent>(pCollision->entityB, "force");
		}


		m_ResolveCollision(pCollision, pTransformA, pTransformB, pForceA, pForceB);
	}

	// Trigger collision event for objects that collided
	m_pCollisionEvent->TriggerCollisions(m_vecCollided);
}

bool Physics::IsRunning()
{
	return m_isRunning;
}

void Physics::SetRunning(bool isRunning)
{
	m_isRunning = isRunning;
}

bool Physics::AABBAABB_Test(sAABB* aabbA, glm::mat4 matTransfA, 
							sAABB* aabbB, glm::mat4 matTransfB, 
							glm::vec3& contactPointA, glm::vec3& contactPointB, 
							glm::vec3& collisionNormalA, glm::vec3& collisionNormalB)
{
	// Transform A in world space
	glm::vec4 AminWorld = (matTransfA * glm::vec4(aabbA->minXYZ, 1.0f));
	glm::vec4 AmaxWorld = (matTransfA * glm::vec4(aabbA->maxXYZ, 1.0f));

	// Transform B in world space
	glm::vec4 BminWorld = (matTransfB * glm::vec4(aabbB->minXYZ, 1.0f));
	glm::vec4 BmaxWorld = (matTransfB * glm::vec4(aabbB->maxXYZ, 1.0f));

	// Check if objects collide
	if (AmaxWorld[0] < BminWorld[0])
	{
		return false;
	}

	if (AminWorld[0] > BmaxWorld[0])
	{
		return false;
	}

	if (AmaxWorld[1] < BminWorld[1])
	{
		return false;
	}

	if (AminWorld[1] > BmaxWorld[1])
	{
		return false;
	}

	if (AmaxWorld[2] < BminWorld[2])
	{
		return false;
	}

	if (AminWorld[2] > BmaxWorld[2])
	{
		return false;
	}

	// Determine the collision side by calculating the minimum overlap in each direction
	float xOverlap = glm::min(AmaxWorld[0], BmaxWorld[0]) - glm::max(AminWorld[0], BminWorld[0]);
	float yOverlap = glm::min(AmaxWorld[1], BmaxWorld[1]) - glm::max(AminWorld[1], BminWorld[1]);
	float zOverlap = glm::min(AmaxWorld[2], BmaxWorld[2]) - glm::max(AminWorld[2], BminWorld[2]);

	// Determine the normal direction based on the collision side
	if (xOverlap < yOverlap && xOverlap < zOverlap) {
		// Horizontal collision
		if (AmaxWorld[0] < BmaxWorld[0]) {
			collisionNormalA = glm::vec3(-1.0f, 0.0f, 0.0f);
			collisionNormalB = glm::vec3(1.0f, 0.0f, 0.0f);
		}
		else {
			collisionNormalA = glm::vec3(1.0f, 0.0f, 0.0f);
			collisionNormalB = glm::vec3(-1.0f, 0.0f, 0.0f);
		}
	}
	else if (yOverlap < xOverlap && yOverlap < zOverlap) {
		// Vertical collision
		if (AmaxWorld[1] < BmaxWorld[1]) {
			collisionNormalA = glm::vec3(0.0f, -1.0f, 0.0f);
			collisionNormalB = glm::vec3(0.0f, 1.0f, 0.0f);
		}
		else {
			collisionNormalA = glm::vec3(0.0f, 1.0f, 0.0f);
			collisionNormalB = glm::vec3(0.0f, -1.0f, 0.0f);
		}
	}
	else {
		// Depth collision (Z-axis)
		if (AmaxWorld[2] < BmaxWorld[2]) {
			collisionNormalA = glm::vec3(0.0f, 0.0f, -1.0f);
			collisionNormalB = glm::vec3(0.0f, 0.0f, 1.0f);
		}
		else {
			collisionNormalA = glm::vec3(0.0f, 0.0f, 1.0f);
			collisionNormalB = glm::vec3(0.0f, 0.0f, -1.0f);
		}
	}

	// Calculate the contact points
	contactPointA = AminWorld;
	contactPointB = BminWorld;

	return true; // Collision detected
}

bool Physics::AABBAABB2D_Test(sAABB2D* aabb2dA, glm::mat4 matTransfA,
								sAABB2D* aabb2dB, glm::mat4 matTransfB,
								glm::vec3& contactPointA, glm::vec3& contactPointB,
								glm::vec3& collisionNormalA, glm::vec3& collisionNormalB)
{
	// Transform A in world space
	glm::vec4 AminWorld = (matTransfA * glm::vec4(aabb2dA->minXY, 1.0f, 1.0f));
	glm::vec4 AmaxWorld = (matTransfA * glm::vec4(aabb2dA->maxXY, 1.0f, 1.0f));

	// Transform B in world space
	glm::vec4 BminWorld = (matTransfB * glm::vec4(aabb2dB->minXY, 1.0f, 1.0f));
	glm::vec4 BmaxWorld = (matTransfB * glm::vec4(aabb2dB->maxXY, 1.0f, 1.0f));

	// Check if objects collide
	if (AmaxWorld[0] < BminWorld[0])
	{
		return false;
	}

	if (AminWorld[0] > BmaxWorld[0])
	{
		return false;
	}

	if (AmaxWorld[1] < BminWorld[1])
	{
		return false;
	}

	if (AminWorld[1] > BmaxWorld[1])
	{
		return false;
	}

	// Determine the collision side by calculating the minimum overlap in each direction
	float xOverlap = glm::min(AmaxWorld[0], BmaxWorld[0]) - glm::max(AminWorld[0], BminWorld[0]);
	float yOverlap = glm::min(AmaxWorld[1], BmaxWorld[1]) - glm::max(AminWorld[1], BminWorld[1]);

	// Determine the normal direction based on the collision side
	if (xOverlap < yOverlap) {
		// Horizontal collision
		if (AmaxWorld[0] < BmaxWorld[0]) {
			collisionNormalA = glm::vec3(-1.0f, 0.0f, 0.0f);
			collisionNormalB = glm::vec3(1.0f, 0.0f, 0.0f);
		}
		else {
			collisionNormalA = glm::vec3(1.0f, 0.0f, 0.0f);
			collisionNormalB = glm::vec3(-1.0f, 0.0f, 0.0f);
		}
	}
	else {
		// Vertical collision
		if (AmaxWorld[1] < BmaxWorld[1]) {
			collisionNormalA = glm::vec3(0.0f, -1.0f, 0.0f);
			collisionNormalB = glm::vec3(0.0f, 1.0f, 0.0f);
		}
		else {
			collisionNormalA = glm::vec3(0.0f, 1.0f, 0.0f);
			collisionNormalB = glm::vec3(0.0f, -1.0f, 0.0f);
		}
	}

	// Calculate the contact points
	contactPointA = glm::vec3(AminWorld[0], AminWorld[1], 0.0f);
	contactPointB = glm::vec3(BminWorld[0], BminWorld[1], 0.0f);

	return true; // Collision detected
}

void Physics::m_ApplyForce(ForceComponent* pForce, TransformComponent* pTransform, double deltaTime)
{
	// Check if object have mass
	if (pForce->GetInverseMass() <= 0.0f)
	{
		return;
	}

	// Explicit forward Euler "integration step"
	//	NewVelocity = LastVel + (Accel * DeltaTime)
	//	NewPosition = LastPos + (Vel * DeltaTime)	

	// Calculate new velocity this frame based on 
	// delta time, acceleration and current velocity
	glm::vec3 velThisFrame = (pForce->GetAcceleration() * (float)deltaTime) + pForce->GetVelocity();
	pForce->SetVelocity(velThisFrame);
	// New object position
	glm::vec3 deltaPosition = velThisFrame * (float)deltaTime;

	pTransform->Move(deltaPosition);

	return;
}

void Physics::m_CheckCollisions(EntityID entityA, CollisionComponent* pCollA, TransformComponent* pTransformA)
{
	using namespace std;
	using namespace glm;

	bool isIntersecting = false;
	vec3 collisionNormalA = vec3(0);
	vec3 collisionNormalB = vec3(0);

	if (!pCollA->IsActive())
	{
		return;
	}

	mat4 transformMatA = pTransformA->GetTransformNoRotation();
	vec3 contactPointA(0);
	vec3 contactPointB(0);

	// Go through list of already visited components, this way we are sure we'll only test 1 time each
	for (EntityID entityB : m_vecCollVisited)
	{
		bool isCollision = false;
		CollisionComponent* pCollB = m_pSceneView->GetComponent<CollisionComponent>(entityB, "collision");
		TransformComponent* pTransformB = m_pSceneView->GetComponent<TransformComponent>(entityB, "transform");
		mat4 transformMatB = pTransformB->GetTransformNoRotation();

		if (pCollA->Get_eBodyType() == eBodyType::STATIC && pCollB->Get_eBodyType() == eBodyType::STATIC)
		{
			// 2 static objects should not trigger
			continue;
		}

		if (pCollA->Get_eShape() == eShape::AABB2D && pCollB->Get_eShape() == eShape::AABB2D)
		{
			sAABB2D* pAABB2D_A = pCollA->GetShape<sAABB2D>();
			sAABB2D* pAABB2D_B = pCollB->GetShape<sAABB2D>();

			isCollision = AABBAABB2D_Test(pAABB2D_A, transformMatA, pAABB2D_B, transformMatB,
				contactPointA, contactPointB, collisionNormalA, collisionNormalB);
		}
		else if (pCollA->Get_eShape() == eShape::AABB && pCollB->Get_eShape() == eShape::AABB)
		{
			sAABB* pAABB_A = pCollA->GetShape<sAABB>();
			sAABB* pAABB_B = pCollB->GetShape<sAABB>();

			isCollision = AABBAABB_Test(pAABB_A, transformMatA, pAABB_B, transformMatB,
				contactPointA, contactPointB, collisionNormalA, collisionNormalB);
		}
		else
		{
			// Collision test not implemented yet
			isCollision = false;
		}

		if (!isCollision)
		{
			continue;
		}

		TagComponent* tagA = m_pSceneView->GetComponent<TagComponent>(entityA, "tag");
		TagComponent* tagB = m_pSceneView->GetComponent<TagComponent>(entityB, "tag");

		// Set all collision data needed for others to handle it
		sCollisionData* pCollision = new sCollisionData();
		pCollision->entityA = entityA;
		pCollision->entityB = entityB;
		pCollision->tagA = tagA->name;
		pCollision->tagB = tagB->name;
		pCollision->bodyTypeA = pCollA->Get_eBodyType();
		pCollision->bodyTypeB = pCollB->Get_eBodyType();
		pCollision->collisionNormalA = collisionNormalA;
		pCollision->collisionNormalB = collisionNormalB;
		pCollision->contactPointA = contactPointA;
		pCollision->contactPointB = contactPointB;
		pCollision->positionA = pTransformA->GetPosition();
		pCollision->positionB = pTransformB->GetPosition();

		// Add to vector that will be later sent as notification
		m_vecCollided.push_back(pCollision);
	}

	m_vecCollVisited.push_back(entityA);

	return;
}

// TODO: Entity A will never be static body here right? because we just jump them
void Physics::m_ResolveCollision(sCollisionData* pCollisionEvent, TransformComponent* pTransformA,
	TransformComponent* pTransformB, ForceComponent* pForceA, ForceComponent* pForceB)
{
	using namespace glm;

	vec3 velocityA = vec3(0);
	vec3 velocityB = vec3(0);

	float inverseMassA = 0;
	float inverseMassB = 0;

	float restitutionA = 0;
	float restitutionB = 0;

	if (pForceA)
	{
		velocityA = pForceA->GetVelocity();
		inverseMassA = pForceA->GetInverseMass();
		restitutionA = pForceA->GetRestitution();
	}

	if (pForceB)
	{
		velocityB = pForceB->GetVelocity();
		inverseMassB = pForceB->GetInverseMass();
		restitutionB = pForceB->GetRestitution();
	}

	// Recalculate velocity based on inverse mass
	if (pCollisionEvent->bodyTypeA == eBodyType::DYNAMIC && pCollisionEvent->bodyTypeB == eBodyType::STATIC)
	{
		myutils::ResolveVelocity(velocityA, velocityB, pCollisionEvent->collisionNormalA, restitutionA,
			inverseMassA, inverseMassB);

		pForceA->SetVelocity(velocityA);
		pTransformA->SetOldPosition(2);
	}

	if (pCollisionEvent->bodyTypeB == eBodyType::DYNAMIC && pCollisionEvent->bodyTypeA == eBodyType::STATIC)
	{
		myutils::ResolveVelocity(velocityB, velocityA, pCollisionEvent->collisionNormalB, restitutionB,
			inverseMassB, inverseMassA);

		pForceB->SetVelocity(velocityB);
		pTransformB->SetOldPosition(2);
	}
}

void Physics::m_DebugCollisions()
{
}