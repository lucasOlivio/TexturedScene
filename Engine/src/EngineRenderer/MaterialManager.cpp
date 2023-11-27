#include "EngineRenderer/MaterialManager.h"
#include "common/utils.h"
#include "components/Texture.h"
#include "common/constants.h"

MaterialManager::MaterialManager(std::string baseTexturesPath)
{
	m_pTextureManager = new cBasicTextureManager();
	m_pTextureManager->SetBasePath(baseTexturesPath);
}

MaterialManager::~MaterialManager()
{
	delete m_pTextureManager;
}

bool MaterialManager::LoadMaterial(SceneView* pScene, MaterialComponent* pMaterial)
{
	using namespace std;

	// Load each color texture
	pMaterial->texturesComponents.clear();
	for (std::string textureName : pMaterial->colorTextures)
	{
		// Bind texture component
		TextureComponent* pTexture = m_LoadTexture(pScene, textureName);
		if (pTexture == nullptr)
		{
			return false;
		}

		pMaterial->texturesComponents.push_back(pTexture);
	}

	// Load heightmap texture
	if (pMaterial->useHeightMap)
	{
		TextureComponent* pTexture = m_LoadTexture(pScene, pMaterial->heightMapTexture);
		if (pTexture == nullptr)
		{
			return false;
		}

		pMaterial->texturesComponents.push_back(pTexture);
	}

	// Load normal texture
	if (pMaterial->useNormalTexture)
	{
		TextureComponent* pTexture = m_LoadTexture(pScene, pMaterial->normalTexture);
		if (pTexture == nullptr)
		{
			return false;
		}

		pMaterial->texturesComponents.push_back(pTexture);
	}

	// Load specular texture
	if (pMaterial->useSpecularTexture)
	{
		TextureComponent* pTexture = m_LoadTexture(pScene, pMaterial->specularTexture);
		if (pTexture == nullptr)
		{
			return false;
		}

		pMaterial->texturesComponents.push_back(pTexture);
	}

	// Load discard texture
	if (pMaterial->useDiscardTexture)
	{
		TextureComponent* pTexture = m_LoadTexture(pScene, pMaterial->discardTexture);
		if (pTexture == nullptr)
		{
			return false;
		}

		pMaterial->texturesComponents.push_back(pTexture);
	}

	// Load cube texture
	if (pMaterial->useCubeTexture)
	{
		TextureComponent* pTexture = m_LoadTexture(pScene, pMaterial->cubeTexture);
		if (pTexture == nullptr)
		{
			return false;
		}

		pMaterial->texturesComponents.push_back(pTexture);
	}

	return true;
}

void MaterialManager::BindMaterial(ShaderManager::ShaderProgram* pShaderProgram, MaterialComponent* pMaterial,
									double deltatime)
{	
	// Only change material if not already binded
	if (pMaterial->materialName == m_currMaterial)
	{
		return;
	}

	UnbindMaterials(pShaderProgram);
	m_currMaterial = pMaterial->materialName;
	std::vector<TextureComponent*> vecTexturesComp = pMaterial->texturesComponents;

	// Update offset and alpha
	UpdateOffset(pShaderProgram, pMaterial, deltatime);
	pShaderProgram->SetUniformFloat("alphaValue", pMaterial->alphaValue);

	// Bind color textures
	for (int i = 0; i < pMaterial->colorTextures.size(); i++)
	{
		m_pTextureManager->BindTexture(pShaderProgram,
									   vecTexturesComp[i]->fileName, 
									   vecTexturesComp[i]->textureType, 
									   pMaterial->colorTexturesRatios[i]);
	}

	// TODO: Remove repetition, all could be in a vector or map
	// Bind heightmap textures
	if (pMaterial->useHeightMap)
	{
		// Height scale can be passed as the "ratio parameter", will be placed on the heightscale uniform
		m_pTextureManager->BindTexture(pShaderProgram,
			pMaterial->heightMapTexture,
			eTextureType::HEIGHTMAP,
			pMaterial->heightScale);
	}

	// Bind normal textures
	if (pMaterial->useNormalTexture)
	{
		m_pTextureManager->BindTexture(pShaderProgram,
			pMaterial->normalTexture,
			eTextureType::NORMAL,
			0);
	}

	// Bind specular textures
	if (pMaterial->useSpecularTexture)
	{
		m_pTextureManager->BindTexture(pShaderProgram,
			pMaterial->specularTexture,
			eTextureType::SPECULAR,
			0);
	}

	// Bind discard textures
	if (pMaterial->useDiscardTexture)
	{
		m_pTextureManager->BindTexture(pShaderProgram,
			pMaterial->discardTexture,
			eTextureType::DISCARD,
			0);
	}

	// Bind cube textures
	if (pMaterial->useCubeTexture)
	{
		m_pTextureManager->BindTexture(pShaderProgram,
			pMaterial->cubeTexture,
			eTextureType::CUBE,
			0);
	}
}

void MaterialManager::UnbindMaterials(ShaderManager::ShaderProgram* pShaderProgram)
{
	m_pTextureManager->ResetSamplers();
	pShaderProgram->SetUniformVec2("UVOffset", glm::vec2(0.0, 0.0));
	pShaderProgram->SetUniformFloat("alphaValue", 1.0f);
}

void MaterialManager::UpdateOffset(ShaderManager::ShaderProgram* pShaderProgram, MaterialComponent* pMaterial, 
								   double deltatime)
{
	if (pMaterial->offsetMove == glm::vec3(0))
	{
		return;
	}

	pMaterial->currOffset += pMaterial->offsetMove * (float)deltatime;

	// Clamp offset between 0 and 1
	if (pMaterial->currOffset.x > 1)
	{
		pMaterial->currOffset.x = 0;
	}
	if (pMaterial->currOffset.y > 1)
	{
		pMaterial->currOffset.y = 0;
	}
	pShaderProgram->SetUniformVec2("UVOffset", pMaterial->currOffset);
}

TextureComponent* MaterialManager::m_LoadTexture(SceneView* pScene, std::string textureName)
{
	// Bind texture component
	TextureComponent* pTexture = pScene->GetComponentByTag<TextureComponent>(textureName, "texture");

	bool isLoaded;
	std::string errorMsg = "";
	if (pTexture->textureType == eTextureType::CUBE)
	{
		if (pTexture->vecTextures.size() < 6)
		{
			CheckEngineError(("Cube Texture " + pTexture->fileName + " don't have enough sides").c_str());
			return nullptr;
		}
		// Load all sides from bitmap files
		isLoaded = m_pTextureManager->CreateCubeTextureFromBMPFiles(pTexture->fileName, 
																    pTexture->vecTextures[0], 
																    pTexture->vecTextures[1], 
																    pTexture->vecTextures[2], 
																    pTexture->vecTextures[3], 
																    pTexture->vecTextures[4], 
																    pTexture->vecTextures[5],
																	true, 
																	errorMsg);
	}
	else
	{
		// Load texture bitmap from file
		isLoaded = m_pTextureManager->Create2DTextureFromBMPFile(pTexture->fileName, true);
		errorMsg = m_pTextureManager->GetLastError();
	}

	if (!isLoaded)
	{
		CheckEngineError(errorMsg.c_str());
		return nullptr;
	}

	return pTexture;
}
