#pragma once

#include "DefaultObject.h"

class TriggerObject : public DefaultObject
{
private:
	std::vector<std::string> m_triggers;

public:
	TriggerObject() {};
	virtual ~TriggerObject() {};

	virtual void OnStart(iEvent* pEvent);

	virtual void Interact(std::string tagThatInteracted, glm::vec3 position);
};
