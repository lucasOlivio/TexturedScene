#include "GameObjects/HangarDoorObject.h"
#include "EngineMedia/MediaPlayer.h"

void HangarDoorObject::OnStart(iEvent* pEvent)
{
	this->DefaultObject::OnStart(pEvent);

	// Script custom
	//----------------
	this->m_isPlaying = std::stoi(this->m_pScript->GetVariable("isPlaying"));
	this->m_speed = myutils::StringToVec3(this->m_pScript->GetVariable("speed"));
	this->m_stopAt = std::stof(this->m_pScript->GetVariable("stopAt"));
}

void HangarDoorObject::Update(double deltaTime)
{
	if (!this->m_isPlaying)
	{
		return;
	}

	if (this->m_elapsedTime > this->m_stopAt)
	{
		this->m_isPlaying = false;
		this->m_elapsedTime = 0;
		this->m_speed *= -1; // Invert direction to toggle close/open
		return;
	}

	this->MoveDoor(deltaTime);
	this->m_elapsedTime += (float)deltaTime;
}

void HangarDoorObject::MoveDoor(double deltaTime)
{
	glm::vec3 deltaMovement = this->m_speed * (float)deltaTime;
	this->m_pTransform->Move(deltaMovement);
}

void HangarDoorObject::Interact(std::string tagThatInteracted, glm::vec3 position)
{
	if (this->m_isPlaying
		|| tagThatInteracted != "hangarDoorTrigger")
	{
		return;
	}

	this->m_isPlaying = true;
}
