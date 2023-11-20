#pragma once

#include "common/types.h"
#include "events/iListener.h"
#include "events/sCollisionData.h"

class iCollisionListener : public iListener
{
public:
	virtual ~iCollisionListener() {};

	virtual void Notify(iEvent* pEvent, sCollisionData* pCollision) = 0;
	virtual void Notify(std::string eventName, iEvent* pEvent) = 0;

	virtual EntityID GetEntityID() = 0;
};
