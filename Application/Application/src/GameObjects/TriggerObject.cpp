#include "GameObjects/TriggerObject.h"

void TriggerObject::OnStart(iEvent* pEvent)
{
	this->DefaultObject::OnStart(pEvent);

	// Script custom
	//----------------
	myutils::SplitString(this->m_pScript->GetVariable("triggers"), ' ', this->m_triggers);
}

void TriggerObject::Interact(std::string tagThatInteracted, glm::vec3 position)
{
	iGameMediator* pGameMediator = this->GetMediator();

	for (std::string triggerTag : this->m_triggers)
	{
		pGameMediator->InteractWith(triggerTag, this->GetTagName(), this->GetPosition());
	}
}
