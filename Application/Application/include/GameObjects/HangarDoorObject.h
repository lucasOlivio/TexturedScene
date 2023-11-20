#pragma once

#include "DefaultObject.h"

class HangarDoorObject : public DefaultObject
{
private:
	// Script custom
	//----------------
	bool m_isPlaying;
	glm::vec3 m_speed; // Animation speed
	float m_stopAt;    // Time to stop

	float m_elapsedTime;

public:
	HangarDoorObject() {};
	virtual ~HangarDoorObject() {};

	void OnStart(iEvent* pEvent);

	virtual void Update(double deltaTime);

	void MoveDoor(double deltaTime);

	virtual void Interact(std::string tagThatInteracted, glm::vec3 position);
};
