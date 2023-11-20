#pragma once

#include <glm/vec3.hpp>
#include "Component.h"

class TagComponent : public Component
{
public:
	std::string name;

	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);
};