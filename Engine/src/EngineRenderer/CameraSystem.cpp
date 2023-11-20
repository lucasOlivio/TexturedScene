#include "EngineRenderer/CameraSystem.h"
#include "common/opengl.h"
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtx/string_cast.hpp>

CameraSystem::CameraSystem(SceneView* pSceneView)
{
    this->m_pSceneView = pSceneView;
}

CameraSystem::~CameraSystem()
{
}

void CameraSystem::Initialize()
{
    this->m_pCamera = this->m_pSceneView->GetComponent<CameraComponent>(0, "camera");
}

glm::mat4 CameraSystem::GetViewMat()
{
    return glm::lookAt(this->m_pCamera->cameraEye,
                        this->m_pCamera->cameraEye + this->m_pCamera->cameraFront,
                        this->m_pCamera->upVector);
}

