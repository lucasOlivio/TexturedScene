#include "EngineRenderer/MaterialManager.h"
#include "common/utils.h"
#include "components/Texture.h"

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

	// Load each ccolor texture
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

	return true;
}

void MaterialManager::BindMaterial(GLuint shaderProgramId, MaterialComponent* pMaterial)
{
	std::vector<TextureComponent*> vecTexturesComp = pMaterial->texturesComponents;

	// Bind color textures
	for (int i = 0; i < pMaterial->colorTextures.size(); i++)
	{
		m_pTextureManager->BindTexture(shaderProgramId, 
									   vecTexturesComp[i]->fileName, 
									   vecTexturesComp[i]->textureType, 
									   pMaterial->colorTexturesRatios[i]);
	}

	// TODO: Remove names dependency, now have to be same name tag and file name to work
	// Bind heightmap textures
	if (pMaterial->useHeightMap)
	{
		// Height scale can be passed as the "ratio parameter", will be placed on the heightscale uniform
		m_pTextureManager->BindTexture(shaderProgramId,
									   pMaterial->heightMapTexture,
									   eTextureType::HEIGHTMAP,
									   pMaterial->heightScale);
	}
}

void MaterialManager::UnbindMaterial(GLuint shaderProgramId)
{
	m_pTextureManager->ResetSamplers(shaderProgramId);
}

TextureComponent* MaterialManager::m_LoadTexture(SceneView* pScene, std::string textureName)
{
	// Bind texture component
	TextureComponent* pTexture = pScene->GetComponentByTag<TextureComponent>(textureName, "texture");

	// Load texture bitmap from file
	bool isLoaded = m_pTextureManager->Create2DTextureFromBMPFile(pTexture->fileName, true);
	if (!isLoaded)
	{
		CheckEngineError(m_pTextureManager->GetLastError().c_str());
		return nullptr;
	}

	return pTexture;
}
