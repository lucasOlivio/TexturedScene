#pragma once

#include "scene/iScene.h"
#include "drawInfo.h"
#include "EngineRenderer/ShaderManager.h"
#include "EngineRenderer/VAOManager.h"
#include "scene/SceneView.h"

enum class eDebugMode
{
	COLLISION = 1 << 0,
	NORMAL = 1 << 1
};

class DebugSystem
{
public:
	~DebugSystem() {};

	static DebugSystem* Get();

	bool Initialize(SceneView* pScene, ShaderManager* pShaderManager, std::string baseModelsPath);

	void ResetDebugObjects();

	// Debug objects
	void AddLine(glm::vec3 startXYZ, glm::vec3 endXYZ, glm::vec4 RGBA);
	void AddGizmo(glm::vec3 position, int size);
	void AddRectangle(glm::vec3 minXYZ, glm::vec3 maxXYZ, glm::vec4 RGBA);
	void AddSphere(glm::vec3 position, float radius, glm::vec4 color);

	void Update(double deltaTime, glm::mat4 matView, glm::mat4 matProjection);

	//https://dietertack.medium.com/using-bit-flags-in-c-d39ec6e30f08
	//Sets the debug mode value from true to false and vice versa
	void ToggleMode(eDebugMode dMode);

	//Check whether multiple modes are set to true
	bool IsModesOn(eDebugMode dModes);

private:
	// Singleton
	DebugSystem() {};
	static DebugSystem* m_pInstance;

	SceneView* m_pScene;

	uint8_t m_debugMode;

	std::string m_debugShaderName;
	uint m_debugShaderID;
	uint m_debugVAO;
	uint m_debugVBO;
	ShaderManager* m_pShaderManager;
	ShaderManager::ShaderProgram* m_pShaderProgram;

	VAOManager* m_pVAOManager;
	sMesh* m_pSphereMesh;

	struct sDebugSphere
	{
		glm::vec3 position;
		glm::vec4 color;
		float radius;
	};

	struct sLineVertex
	{
		float x, y, z, w;
		float r, g, b, a;
	};

	std::vector<sDebugSphere*> m_vecSpheresToDraw;

	int m_sizeOfLineVBO;
	sLineVertex* m_pLinesVerticesToDraw;

	void m_InitializeLineVertex();

	void m_AddCollisions();
	void m_AddNormals();

	void m_DrawLines();
	void m_DrawSpheres();
};
