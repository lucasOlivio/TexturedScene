#ifndef _cBasicTextureManager_HG_
#define _cBasicTextureManager_HG_

#include "CTextureFromBMP.h"
#include "common/types.h"
#include "EngineRenderer/TextureProperties.h"
#include "EngineRenderer/ShaderManager.h"
#include <string>
#include <map>

struct sSamplerInfo
{
	GLint samplerRatioUL;
	GLint samplerToggleUL;
	GLint samplerUL;
	GLint samplerId;
};

class cBasicTextureManager
{
public:
	cBasicTextureManager();
	~cBasicTextureManager();

	bool Create2DTextureFromBMPFile( std::string textureFileName, bool bGenerateMIPMap );

	// Picks a random texture from the textures loaded
	std::string PickRandomTexture(void);

	bool CreateCubeTextureFromBMPFiles( std::string cubeMapName, 
		                                std::string posX_fileName, std::string negX_fileName, 
		                                std::string posY_fileName, std::string negY_fileName, 
										std::string posZ_fileName, std::string negZ_fileName, 
										bool bIsSeamless, std::string &errorString );


	// returns 0 on error
	GLuint GetTextureIDFromName( std::string textureFileName );

	// gets the sampler to be used based on the texture type
	GLuint GetSamplerId(eTextureType textureType);

	sSamplerInfo* GetSamplerInfo(ShaderManager::ShaderProgram* pShaderProgram, eTextureType textureTypeIn);
	sSamplerInfo* GetSamplerInfoFromName(std::string samplerName);

	// If texture not already in any sampler, then loads into next available
	void BindTexture(ShaderManager::ShaderProgram* pShaderProgram, std::string textureName, 
					 eTextureType textureType, float ratio);
	
	// Reset all ratios to 0
	void ResetSamplers();

	void SetBasePath(std::string basepath);

	std::string GetLastError();

private:
	std::string m_basePath;
	std::string m_lastError;
	void m_AppendErrorString( std::string nextErrorText );
	void m_AppendErrorStringLine( std::string nextErrorTextLine );

	std::map< std::string, CTextureFromBMP* > m_mapTexNameToTexture;
	std::map< std::string /*sampler name*/, sSamplerInfo* > m_mapSamplerNameToInfo;

	int m_nextColorSamplerId;
};

#endif
