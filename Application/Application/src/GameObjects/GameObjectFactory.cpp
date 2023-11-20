#include "GameObjects/GameObjectFactory.h"
#include "GameObjects/GameManager.h"
#include "GameObjects/Player.h"
#include "GameObjects/AudioObject.h"
#include "GameObjects/HangarDoorObject.h"
#include "GameObjects/TriggerObject.h"
#include "GameObjects/AlienObject.h"

GameObjectFactory::GameObjectFactory(SceneView* pScene)
{
	this->m_pScene = pScene;
}

iGameObject* GameObjectFactory::CreateGameObject(std::string gameObjName, bool isCopy)
{
	iGameObject* pGameObj;

	EntityID entityID = this->m_pScene->GetEntity(gameObjName);

	// Should we use the entity itself or just a copy of it?
	if (isCopy)
	{
		entityID = this->m_pScene->CreateEntity(entityID);
	}

	ScriptComponent* pScript = this->m_pScene->GetComponent<ScriptComponent>(entityID, "script");
	std::string scriptName = pScript->GetScriptName();

	if (scriptName == "GameManager")
	{
		pGameObj = new GameManager();
	}
	else if (scriptName == "Player")
	{
		pGameObj = new Player();
	}
	else if (scriptName == "AudioObject")
	{
		pGameObj = new AudioObject();
	}
	else if (scriptName == "HangarDoorObject")
	{
		pGameObj = new HangarDoorObject();
	}
	else if (scriptName == "TriggerObject")
	{
		pGameObj = new TriggerObject();
	}
	else if (scriptName == "AlienObject")
	{
		pGameObj = new AlienObject();
	}
	else
	{
		// Should not return null
		assert(nullptr);
		return nullptr;
	}

	bool isLoaded = pGameObj->Load(entityID, this->m_pScene);
	if (!isLoaded)
	{
		delete pGameObj;
		return nullptr;
	}

	return pGameObj;
}
