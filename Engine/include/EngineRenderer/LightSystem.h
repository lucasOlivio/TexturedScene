#pragma once

#include "scene/SceneView.h"
#include "iShaderInfo.h"
#include <glm/mat4x4.hpp>

class LightSystem
{
private:
	iShaderInfo* m_pShaderInfo;
	SceneView* m_pSceneView;
public:
	// ctors & dtors
	LightSystem(iShaderInfo* pShaderInfo, SceneView* pSceneView);
	~LightSystem();

	bool Initialize(int shaderID);

	void Update(double deltatime);
};