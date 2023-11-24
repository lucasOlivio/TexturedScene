#pragma once

#include "Component.h"
#include "EngineRenderer/TextureProperties.h"
#include <glm/vec3.hpp>

class TextureComponent : public Component
{
public:
	std::string fileName;
	eTextureType textureType;

	// Cube map list of texture sides
	std::vector<std::string> vecTextures;

	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);
};