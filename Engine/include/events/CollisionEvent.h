#pragma once

#include "Event.h"
#include "common/types.h"
#include "events/sCollisionData.h"
#include <glm/vec3.hpp>
#include <map>

// Collision events triggered by physics engine
class CollisionEvent : public Event
{
public:
	CollisionEvent();
	virtual ~CollisionEvent();

	// Send event to those that collided in frame
	void TriggerCollisions(std::vector<sCollisionData*> vecFrameCollisionsIn);
};