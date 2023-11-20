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

	glm::vec3 GetCameraFront(const glm::vec3& cameraPosition, const glm::vec3& rotation);

	// Calculate the camera target based on position and rotation
	glm::vec3 GetCameraTarget(const glm::vec3& cameraPosition, const glm::vec3& rotation);
};