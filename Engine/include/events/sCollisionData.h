#pragma once

#include "common/types.h"
#include "EnginePhysics/PhysicsProperties.hpp"

struct sCollisionData
{
	EntityID entityA;
	EntityID entityB;
	std::string tagA;
	std::string tagB;
	eBodyType bodyTypeA;
	eBodyType bodyTypeB;
	sTriangle* pMeshTriangleCollision;
	glm::vec3 contactPointA;
	glm::vec3 contactPointB;
	glm::vec3 collisionNormalA;
	glm::vec3 collisionNormalB;
	glm::vec3 positionA;
	glm::vec3 positionB;
	glm::vec3 velocityAtCollisionA;
	glm::vec3 velocityAtCollisionB;
};