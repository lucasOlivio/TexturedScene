#pragma once

#include "Component.h"
#include "EngineRenderer/TextureProperties.h"
#include <glm/vec3.hpp>

class TextureComponent : public Component
{
public:
	std::string fileName;
	eTextureType textureType;

	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);
};