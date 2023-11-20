#pragma once

#include <glm/vec3.hpp>
#include "Component.h"

class TextureComponent : public Component
{
public:
	// TODO: Maybe a vector for file name and sampler id to have multiple for one model?
	std::string fileName;
	int samplerId;
	bool generateMIPMap;

	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);
};