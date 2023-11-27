#include "components/Camera.h"
#include "common/utils.h"
#include "common/constants.h"
#include "common/utilsMat.h"
#include <glm/gtx/string_cast.hpp>

void CameraComponent::GetInfo(sComponentInfo& compInfoOut)
{
	using namespace myutils;

	compInfoOut.componentName = "camera";
	compInfoOut.componentParameters.clear();

    this->AddCompParInfo("upVector", "vec3", this->upVector, compInfoOut);
}

void CameraComponent::SetParameter(sParameterInfo& parameterIn)
{
    using namespace myutils;

    if (parameterIn.parameterName == "upVector") {
        this->upVector = parameterIn.parameterVec3Value;
    }

    return;
}

glm::vec3 CameraComponent::GetCameraFront(const glm::vec3& cameraPosition, const glm::vec3& rotation)
{
	glm::vec3 front = myutils::GetDirectionFromDegrees(rotation);

	return front;
}

glm::vec3 CameraComponent::GetCameraTarget(const glm::vec3& cameraPosition, const glm::vec3& rotation)
{
	return cameraPosition + GetCameraFront(cameraPosition, rotation);
}
