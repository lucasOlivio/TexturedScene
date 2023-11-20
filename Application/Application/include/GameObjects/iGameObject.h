#pragma once

#include "common/types.h"
#include "scene/SceneView.h"
#include "iGameMediator.h"
#include "events/iCollisionListener.h"
#include "events/iEvent.h"

class iGameObject : public iCollisionListener
{
public:
	virtual ~iGameObject() {};

	// Load all info and components needed from scene
	virtual bool Load(EntityID entityID, SceneView* pScene) = 0;

	virtual void OnStart(iEvent* pEvent) = 0;

	virtual void Update(double deltaTime) = 0;

	virtual void OnExit(iEvent* pEvent) = 0;

	virtual EntityID GetEntityID() = 0;

	virtual void SetMediator(iGameMediator* pMediator) = 0;
	virtual iGameMediator* GetMediator() = 0;

	virtual void SetPosition(glm::vec3 value) = 0;
	virtual void SetDirection(glm::vec3 value) = 0;

	virtual glm::vec3 GetPosition() = 0;
	virtual std::string GetTagName() = 0;
	virtual std::string GetScriptName() = 0;

	virtual void Notify(iEvent* pEvent, sCollisionData* pCollision) = 0;
	virtual void Notify(iEvent* pEvent) = 0;

	// Will remove from list of objects on update
	virtual void Delete() = 0;
	virtual bool IsDeleted() = 0;

	virtual void Interact(std::string tagThatInteracted, glm::vec3 position) = 0;
};
