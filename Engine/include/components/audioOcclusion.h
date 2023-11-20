#pragma once

#include "Component.h"
#include <glm/vec3.hpp>
#include <fmod/fmod.hpp>

class AudioOcclusionComponent : public Component
{
private:
	float m_direct;
	float m_reverb;

public:
	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);

	float GetDirect();
	float GetReverb();
};