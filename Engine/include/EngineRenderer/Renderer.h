#pragma once

#include "ShaderManager.h"
#include "WindowSystem.h"
#include "CameraSystem.h"
#include "ModelSystem.h"
#include "LightSystem.h"
#include "TextureManager/cBasicTextureManager.h"
#include "scene/SceneView.h"

class Renderer
{
private:
	bool m_isRunning;
	bool m_isInitialized;

	ShaderManager* m_pShaderManager;
	ShaderManager::ShaderProgram* m_pShaderProgram;
	uint m_currShaderID;

	CameraSystem* m_pCameraSystem;
	ModelSystem* m_pModelSystem;
	LightSystem* m_pLightSystem;

	cBasicTextureManager* m_pTextureManager;

	SceneView* m_pSceneView;

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

	bool LoadTextures();

	void BindTexture(std::string fileName, uint samplerId);

	void UpdateCamera();
	void RenderAllModels(double deltaTime);

	// Update respectives UL and render model
	void RenderScene(double deltaTime);
	// Update transform values in shaders uniforms
	void UpdateTransform(glm::mat4 matModel);
	// Uses opengl to draw the vao
	void Draw(bool isWireFrame,
			  bool doNotLight,
			  bool useVertexColour,
			  int VAO_ID,
			  int numberOfIndices);

	bool IsRunning();

	CameraSystem* GetCamera();
};