#pragma once

#include "Component.h"
#include "Texture.h"
#include <glm/vec3.hpp>

class MaterialComponent : public Component
{
public:
	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);

	std::string materialName;

	glm::vec3 currOffset;
	glm::vec3 offsetMove;

	std::vector<std::string> colorTextures;
	glm::vec3 colorTexturesRatios;

	bool useHeightMap;
	std::string heightMapTexture;
	float heightScale;

	bool useNormalTexture;
	std::string normalTexture;

	bool useSpecularTexture;
	std::string specularTexture;

	bool useDiscardTexture;
	std::string discardTexture;

	bool useCubeTexture;
	std::string cubeTexture;

	// Material transparency
	float alphaValue;

	std::vector<TextureComponent*> texturesComponents;
};
