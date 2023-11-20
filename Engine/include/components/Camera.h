#pragma once

#include "Component.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class CameraComponent : public Component
{
public:
	glm::vec3 upVector;

	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);

	// Calculate the camera front target based on position and rotation
	glm::vec3 GetCameraFront(const glm::vec3& cameraPosition, const glm::vec3& rotation);
};