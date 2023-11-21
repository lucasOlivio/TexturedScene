#pragma once

#include "scene/iScene.h"
#include "drawInfo.h"
#include "EngineRenderer/ShaderManager.h"
#include "EngineRenderer/VAOManager.h"

class DebugSystem
{
public:
	~DebugSystem() {};

	static DebugSystem* Get();

	bool Initialize(ShaderManager* pShaderManager, std::string baseModelsPath);

	void ResetDebugObjects();

	// Debug objects
	void AddLine(glm::vec3 startXYZ, glm::vec3 endXYZ, glm::vec4 RGBA);
	void AddGizmo(glm::vec3 position, int size);

	void Update(double deltaTime, glm::mat4 matView, glm::mat4 matProjection);
private:
	// Singleton
	DebugSystem() {};
	static DebugSystem* m_pInstance;

	ShaderManager* m_pShaderManager;
	ShaderManager::ShaderProgram* m_pShaderProgram;
	VAOManager* m_pVAOManager;
	std::string m_debugShaderName;
	uint m_debugShaderID;
	uint m_debugVAO;
	uint m_debugVBO;

	struct sLineVertex
	{
		float x, y, z, w;
		float r, g, b, a;
	};

	unsigned int m_sizeOfLineVBO;
	sLineVertex* m_pLinesVerticesToDraw;

	void m_InitializeLineVertex();

	void m_DrawLines();
};
