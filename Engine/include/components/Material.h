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

	std::vector<std::string> colorTextures;
	glm::vec3 colorTexturesRatios;

	bool useHeightMap;
	std::string heightMapTexture;
	float heightScale;

	std::vector<TextureComponent*> texturesComponents;
};
