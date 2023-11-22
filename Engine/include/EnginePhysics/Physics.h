#pragma once

#include "scene/SceneView.h"
#include "events/CollisionEvent.h"
#include "components/Transform.h"
#include "components/Model.h"
#include "components/Force.h"
#include "components/Collision.h"

class Physics
{
public:
	Physics(SceneView* pSceneView, CollisionEvent* pCollisionEvent);
	~Physics();

	// Resets collisions
	void NewFrame();

	// Physics update per frame
	void Update(double deltaTime);

	bool IsRunning();
	void SetRunning(bool isRunning);

	// Collision tests
	// Returns the collision points for each body

	bool AABBAABB_Test(sAABB* aabbA, glm::mat4 matTransfA,
					   sAABB* aabbB, glm::mat4 matTransfB,
					   glm::vec3& contactPointA, glm::vec3& contactPointB,
					   glm::vec3& collisionNormalA, glm::vec3& collisionNormalB);

	bool AABBAABB2D_Test(sAABB2D* aabb2dA, glm::mat4 matTransfA,
						 sAABB2D* aabb2dB, glm::mat4 matTransfB,
						 glm::vec3& contactPointA, glm::vec3& contactPointB,
						 glm::vec3& collisionNormalA, glm::vec3& collisionNormalB);
private:
	bool m_isRunning;

	SceneView* m_pSceneView;
	CollisionEvent* m_pCollisionEvent;

	// All the entities that we already tested in the frame (true or false)
	std::vector<EntityID> m_vecCollVisited;

	// Collisions that happenned in the frame
	std::vector<sCollisionData*> m_vecCollided;

	// Update object transform based on velocity and acceleration
	void m_ApplyForce(ForceComponent* pForce, TransformComponent* pTransform, double deltaTime);

	// Check every object in scene for collision between the collisionShapes
	// Add collision to map cache and send collision events
	void m_CheckCollisions(EntityID entityA, CollisionComponent* pCollA, TransformComponent* pTransformA);

	// Given the collision, calculates the new positions and velocities
	void m_ResolveCollision(sCollisionData* pCollisionEvent, TransformComponent* pTransformA,
		TransformComponent* pTransformB, ForceComponent* pForceA, ForceComponent* pForceB);

	// TODO: When on game release remove these debug codes
	void m_DebugCollisions();
};
