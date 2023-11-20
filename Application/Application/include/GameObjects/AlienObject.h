#pragma once

#include "AudioObject.h"
#include "components/Force.h"

class AlienObject : public AudioObject
{
protected:
	ForceComponent* m_pForce;

	glm::vec3 m_speed;

public:
	AlienObject() {};
	virtual ~AlienObject() {};

	virtual void Update(double deltaTime);

	virtual bool Load(EntityID entityID, SceneView* pScene);

	virtual void OnStart(iEvent* pEvent);

	virtual void Interact(std::string tagThatInteracted, glm::vec3 position);

	// Audio object
	//------------------
	virtual glm::vec3 GetVelocity();
};
