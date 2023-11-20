#include "EngineRenderer/CameraSystem.h"
#include "common/opengl.h"
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtx/string_cast.hpp>

CameraSystem::CameraSystem(SceneView* pSceneView) : 
    m_pCamera(nullptr), m_pTransform(nullptr)
{
    m_pSceneView = pSceneView;
}

CameraSystem::~CameraSystem()
{
}

void CameraSystem::Initialize()
{
    m_pCamera = m_pSceneView->GetComponent<CameraComponent>(0, "camera");
    m_pTransform = m_pSceneView->GetComponent<TransformComponent>(0, "transform");
}

glm::mat4 CameraSystem::GetViewMat()
{
    using namespace glm;

    // Calculating camera view
    vec3 cameraPosition = this->m_pTransform->GetPosition();
    vec3 cameraRotation = this->m_pTransform->GetOrientation();
    vec3 cameraUpVector = this->m_pCamera->upVector;

    vec3 cameraFront = this->m_pCamera->GetCameraFront(cameraPosition, cameraRotation);

    return lookAt(cameraPosition,
                  cameraFront,
                  cameraUpVector);
}

