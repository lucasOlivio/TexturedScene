#pragma once

#include "TextureManager/cBasicTextureManager.h"
#include "scene/SceneView.h"
#include "components/Material.h"

class MaterialManager
{
public:
	MaterialManager(std::string baseTexturesPath);
	~MaterialManager();
	
	// Load material textures
	bool LoadMaterial(SceneView* pScene, MaterialComponent* pMaterial);

	void BindMaterial(GLuint shaderProgramId, MaterialComponent* pMaterial);
	void UnbindMaterial(GLuint shaderProgramId);
private:
	cBasicTextureManager* m_pTextureManager;

	// Load bitmap texture from file using texture manager
	TextureComponent* m_LoadTexture(SceneView* pScene, std::string textureName);
};
