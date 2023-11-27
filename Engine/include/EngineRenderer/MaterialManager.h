#pragma once

#include "TextureManager/cBasicTextureManager.h"
#include "scene/SceneView.h"
#include "components/Material.h"
#include "ShaderManager.h"

class MaterialManager
{
public:
	MaterialManager(std::string baseTexturesPath);
	~MaterialManager();
	
	// Load material textures
	bool LoadMaterial(SceneView* pScene, MaterialComponent* pMaterial);

	void BindMaterial(ShaderManager::ShaderProgram* pShaderProgram, MaterialComponent* pMaterial, double deltatime);
	void UnbindMaterials(ShaderManager::ShaderProgram* pShaderProgram);

	// TODO: Offset movement could be in an animation system that would also handle the models animations
	void UpdateOffset(ShaderManager::ShaderProgram* pShaderProgram, MaterialComponent* pMaterial, double deltatime);
private:
	cBasicTextureManager* m_pTextureManager;
	// Textures and variables in the uniforms and units at the moment
	std::string m_currMaterial;

	// Load bitmap texture from file using texture manager
	TextureComponent* m_LoadTexture(SceneView* pScene, std::string textureName);
};
