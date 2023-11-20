#pragma once

#include "iGameObject.h"

class GameObjectFactory
{
private:
	SceneView* m_pScene;
public:
	GameObjectFactory(SceneView* pScene);

	iGameObject* CreateGameObject(std::string gameObjName, bool isCopy);
};
