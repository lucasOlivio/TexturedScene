#include "GameObjects/AlienObject.h"
#include "EngineMedia/MediaPlayer.h"

void AlienObject::Update(double deltaTime)
{
	if (!this->m_isPlaying)
	{
		return;
	}

	// Update current audio position
	MediaPlayer::Get()->SetAudio3DAttributes(this->GetEntityID(),
											 this->GetPosition(),
											 this->GetVelocity());
}

bool AlienObject::Load(EntityID entityID, SceneView* pScene)
{
	this->AudioObject::Load(entityID, pScene);

	this->m_pForce = pScene->GetComponent<ForceComponent>(entityID, "force");

	return true;
}

void AlienObject::OnStart(iEvent* pEvent)
{
	this->AudioObject::OnStart(pEvent);

	// Script custom
	//----------------
	this->m_speed = myutils::StringToVec3(this->m_pScript->GetVariable("speed"));
}

void AlienObject::Interact(std::string tagThatInteracted, glm::vec3 position)
{
	if (tagThatInteracted != "hangarDoorTrigger")
	{
		return;
	}

	this->m_pForce->SetVelocity(this->m_speed);
	this->AudioObject::Interact(tagThatInteracted, position);
}

glm::vec3 AlienObject::GetVelocity()
{
	return this->m_pForce->GetVelocity();
}
