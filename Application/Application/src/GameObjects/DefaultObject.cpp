#include "GameObjects/DefaultObject.h"
#include "GameObjects/GameObjectFactory.h"
#include "common/utilsMat.h"

bool DefaultObject::Load(EntityID entityID, SceneView* pScene)
{
	this->m_entityID = entityID;

	this->m_pTag = pScene->GetComponent<TagComponent>(entityID, "tag");
	this->m_pTransform = pScene->GetComponent<TransformComponent>(entityID, "transform");
	this->m_pScript = pScene->GetComponent<ScriptComponent>(entityID, "script");

	return true;
}

void DefaultObject::OnStart(iEvent* pEvent)
{
	this->m_currDirection = glm::vec3(0);

	pEvent->Attach(this);
}

void DefaultObject::OnExit(iEvent* pEvent)
{
	pEvent->Dettach(this);
}

EntityID DefaultObject::GetEntityID()
{
	return this->m_entityID;
}

void DefaultObject::Delete()
{
	this->m_isDeleted = true;
}

bool DefaultObject::IsDeleted()
{
	if (this->m_isDeleted)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void DefaultObject::SetMediator(iGameMediator* pMediator)
{
	this->m_pMediator = pMediator;
}

iGameMediator* DefaultObject::GetMediator()
{
	return this->m_pMediator;
}

void DefaultObject::SetPosition(glm::vec3 value)
{
	this->m_pTransform->SetPosition(value);
}

void DefaultObject::SetDirection(glm::vec3 value)
{
	using namespace glm;
	using namespace myutils;

	vec3 rotation = vec3(0.0f, 0.0f, GetAngleToDirection(value.x, value.y));
	this->m_pTransform->SetOrientation(rotation);

	this->m_currDirection = value;
}

glm::vec3 DefaultObject::GetPosition()
{
	return this->m_pTransform->GetPosition();
}

std::string DefaultObject::GetTagName()
{
	return this->m_pTag->name;
}

std::string DefaultObject::GetScriptName()
{
	return this->m_pScript->GetScriptName();
}
