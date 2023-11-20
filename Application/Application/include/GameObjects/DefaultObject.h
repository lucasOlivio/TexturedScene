#pragma once

#include "iGameObject.h"
#include "components/Tag.h"
#include "components/Transform.h"
#include "components/Collision.h"
#include "components/Script.h"
#include "components/Model.h"
#include "events/iCollisionListener.h"

class DefaultObject : public iGameObject
{
protected:
	EntityID m_entityID;
	TagComponent* m_pTag;
	ScriptComponent* m_pScript;
	TransformComponent* m_pTransform;

	iGameMediator* m_pMediator;

	glm::vec3 m_currDirection;

	bool m_isDeleted;

public:
	DefaultObject() {};
	virtual ~DefaultObject() {};

	// Game object
	//----------------
	virtual bool Load(EntityID entityID, SceneView* pScene);

	virtual void OnStart(iEvent* pEvent);

	virtual void Update(double deltaTime) {};

	virtual void OnExit(iEvent* pEvent);

	virtual EntityID GetEntityID();

	virtual void SetMediator(iGameMediator* pMediator);
	virtual iGameMediator* GetMediator();

	virtual void SetPosition(glm::vec3 value);
	virtual void SetDirection(glm::vec3 value);

	virtual glm::vec3 GetPosition();
	virtual std::string GetTagName();
	virtual std::string GetScriptName();

	virtual void Delete();
	virtual bool IsDeleted();

	virtual void Interact(std::string tagThatInteracted, glm::vec3 position) {};

	// Collision listener
	//----------------
	virtual void Notify(iEvent* pEvent, sCollisionData* pCollision) {};
	virtual void Notify(iEvent* pEvent) {};
};
