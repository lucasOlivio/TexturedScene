#include "GameObjects/GameManager.h"
#include "GameObjects/GameObjectFactory.h"
#include "common/utils.h"
#include "common/utilsMat.h"
#include "components/Transform.h"

GameManager::~GameManager()
{
}

void GameManager::GameOver()
{
}

bool GameManager::Load(EntityID entityID, SceneView* pScene)
{
	this->m_pSceneView = pScene;
	this->m_entityID = entityID;

	this->m_pTag = pScene->GetComponent<TagComponent>(entityID, "tag");
	this->m_pScript = pScene->GetComponent<ScriptComponent>(entityID, "script");

	return true;
}

void GameManager::OnStart(iEvent* pEvent)
{
	using namespace std;
	using namespace myutils;
	using namespace glm;

	this->m_pCollisionEvents = pEvent;
	this->m_vecGameObjects.clear();

	// Script custom
	//----------------
	this->m_maxHeight = std::stoi(this->m_pScript->GetVariable("maxHeight"));
	this->m_maxWidth = std::stoi(this->m_pScript->GetVariable("maxWidth"));
	this->m_minHeight = std::stoi(this->m_pScript->GetVariable("minHeight"));
	this->m_minWidth = std::stoi(this->m_pScript->GetVariable("minWidth"));
	this->m_minDistanceToInteract = std::stof(this->m_pScript->GetVariable("minDistanceToInteract"));

	GameObjectFactory gameObjFactory(this->m_pSceneView);

	// Load player first
	//----------------
	this->SpawnGameObj("player");

	// Load rest of game objects
	//----------------
	for (this->m_pSceneView->First("script"); !this->m_pSceneView->IsDone(); this->m_pSceneView->Next())
	{
		EntityID entityId = this->m_pSceneView->CurrentKey();
		TagComponent* pTag = this->m_pSceneView->GetComponent<TagComponent>(entityId, "tag");

		if (pTag->name == "player" || pTag->name == "GM")
		{
			continue;
		}

		this->SpawnGameObj(pTag->name);
	}
}

void GameManager::Update(double deltaTime)
{
	std::vector<int> toDelete;

	for (int i = 0; i < this->m_vecGameObjects.size(); i++)
	{
		iGameObject* pGameObject = this->m_vecGameObjects[i];

		if (pGameObject->IsDeleted())
		{
			toDelete.push_back(i);
			continue;
		}
		this->m_vecGameObjects[i]->Update(deltaTime);
	}

	// Clear deleted game objects
	for (int i : toDelete)
	{
		this->m_vecGameObjects.erase(this->m_vecGameObjects.begin() + i);
	}
}

void GameManager::OnExit(iEvent* pEvent)
{
	for (iGameObject* pGameObj : this->m_vecGameObjects)
	{
		pGameObj->OnExit(pEvent);
		delete pGameObj;
	}
}

std::string GameManager::GetTagName()
{
	return this->m_pTag->name;
}

std::string GameManager::GetScriptName()
{
	return this->m_pScript->GetScriptName();
}

EntityID GameManager::GetEntityID()
{
	return this->m_entityID;
}

iGameObject* GameManager::GetGameObjectByName(std::string tagName)
{
	for (iGameObject* pGameObj : this->m_vecGameObjects)
	{
		if (pGameObj->GetTagName() == tagName)
		{
			return pGameObj;
		}
	}

	return nullptr;
}

bool GameManager::GetTargetDirection(std::string targetName, glm::vec3 objPosition, glm::vec3& directionOut)
{
	iGameObject* pGameObject = this->GetGameObjectByName(targetName);

	if (pGameObject == nullptr)
	{
		return false;
	}

	glm::vec3 targetPosition = pGameObject->GetPosition();
	directionOut = myutils::GetCardinalDirection(targetPosition, objPosition);

	return true;
}

void GameManager::DestroyGameObj(iGameObject* pGameObj)
{
	using namespace std;

	string tagName = pGameObj->GetTagName();

	if (tagName == "player")
	{
		this->GameOver();
	}

	pGameObj->Delete();
	// Clean from scene
	this->m_pSceneView->DeleteEntity(pGameObj->GetEntityID());
	this->m_pSceneView->DeleteListener(pGameObj);
}

void GameManager::Interact(std::string tagThatInteracted, glm::vec3 position)
{
	iGameObject* pGameObjectInteract = nullptr;
	float minDistance = this->m_minDistanceToInteract;

	for (int i = 0; i < this->m_vecGameObjects.size(); i++)
	{
		iGameObject* pGameObject = this->m_vecGameObjects[i];
		float distance = glm::length(pGameObject->GetPosition() - position);

		// Only closest game object gets the interaction
		if (distance < minDistance &&
			tagThatInteracted != pGameObject->GetTagName())
		{
			minDistance = distance;
			pGameObjectInteract = pGameObject;
		}
	}

	if (pGameObjectInteract == nullptr)
	{
		return;
	}

	pGameObjectInteract->Interact(tagThatInteracted, position);
}

void GameManager::InteractWith(std::string tagToInteract, std::string tagThatInteracted, glm::vec3 position)
{
	iGameObject* pGameObject = this->GetGameObjectByName(tagToInteract);

	if (pGameObject == nullptr)
	{
		return;
	}

	pGameObject->Interact(tagThatInteracted, position);
}

void GameManager::SpawnGameObj(std::string tagName, glm::vec3 position, glm::vec3 direction)
{
	using namespace glm;
	using namespace myutils;

	GameObjectFactory gameObjFactory(this->m_pSceneView);

	iGameObject* pGameObj = gameObjFactory.CreateGameObject(tagName, true);

	pGameObj->SetMediator(this);
	pGameObj->OnStart(this->m_pCollisionEvents);

	pGameObj->SetPosition(position);
	pGameObj->SetDirection(direction);

	this->m_vecGameObjects.push_back(pGameObj);
}

void GameManager::SpawnGameObj(std::string tagName)
{
	using namespace glm;
	using namespace myutils;

	GameObjectFactory gameObjFactory(this->m_pSceneView);

	iGameObject* pGameObj = gameObjFactory.CreateGameObject(tagName, false);

	pGameObj->SetMediator(this);
	pGameObj->OnStart(this->m_pCollisionEvents);

	this->m_vecGameObjects.push_back(pGameObj);
}

void GameManager::SpawnGameObjRandomPos(std::string tagName)
{
	using namespace glm;
	using namespace myutils;

	// Avoid enemy been placed at player position
	float playerCenterMin = 450;
	float playerCenterMax = 550;

	vec3 position(0);
	position.x = GetRandFloat((float)this->m_minWidth, (float)this->m_maxWidth, playerCenterMin, playerCenterMax);
	position.y = GetRandFloat((float)this->m_minHeight, (float)this->m_maxHeight, playerCenterMin, playerCenterMax);

	this->SpawnGameObj(tagName, position, vec3(0));
}
