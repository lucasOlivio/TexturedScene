#pragma once

#include <glm/vec3.hpp>
#include "Component.h"

class CameraComponent : public Component
{
public:
	glm::vec3 cameraEye;
	glm::vec3 cameraFront;
	glm::vec3 upVector;
	bool isActive;

	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);
};