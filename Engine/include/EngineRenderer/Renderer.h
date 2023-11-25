#pragma once

#include "ShaderManager.h"
#include "WindowSystem.h"
#include "CameraSystem.h"
#include "ModelSystem.h"
#include "LightSystem.h"
#include "MaterialManager.h"
#include "scene/SceneView.h"
#include "components/Model.h"
#include "components/Transform.h"

class Renderer
{
public:
	// ctors & dtors
	Renderer();
	~Renderer();

	// TODO: The scene should be changed so that the systems can get only the
	// componen they need, to avoid the whole thing been passed everywhere 

	// Initializes all the systems, if anything go wrong we should stop here
	bool Initialize(std::string baseModelsPath,
					std::string baseTexturesPath,
					ShaderManager* pShaderManager,
					uint currShaderID,
					SceneView* pSceneView);
	void Destroy();

	// Go through the scene loading each model component to the VAO
	bool LoadScene(std::string baseModelsPath);

	bool LoadMaterials();

	void UpdateCamera();
	void RenderAllModels(double deltaTime);
	void RenderModel(EntityID entityID, ModelComponent* pModel, TransformComponent* pTransform, double deltaTime);

	// Update respectives UL and render model
	void RenderScene(double deltaTime);
	// Update transform values in shaders uniforms
	void UpdateTransform(glm::mat4 matModel);
	// Uses opengl to draw the vao
	void Draw(bool isWireFrame,
			  bool doNotLight,
			  bool useColorTexture,
			  int VAO_ID,
			  int numberOfIndices);

	bool IsRunning();

	CameraSystem* GetCamera();

private:
	bool m_isRunning;
	bool m_isInitialized;

	ShaderManager* m_pShaderManager;
	ShaderManager::ShaderProgram* m_pShaderProgram;
	uint m_currShaderID;

	CameraSystem* m_pCameraSystem;
	ModelSystem* m_pModelSystem;
	LightSystem* m_pLightSystem;

	MaterialManager* m_pMaterialManager;

	SceneView* m_pSceneView;
};