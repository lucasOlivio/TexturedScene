#include "EngineRenderer/LightSystem.h"
#include "components/Light.h"
#include "components/Transform.h"
#include "common/utilsMat.h"
#include <glm/gtc/quaternion.hpp>

LightSystem::LightSystem(iShaderInfo* pShaderInfo, SceneView* pSceneView)
{
    m_pShaderInfo = pShaderInfo;
    m_pSceneView = pSceneView;
}

LightSystem::~LightSystem()
{
}

bool LightSystem::Initialize(int shaderID)
{
    printf("Setting up lights...\n");
    // Go over each lights setting the ULs and initializing them
    int lightIndex = 0;
    for (m_pSceneView->First("light"); !m_pSceneView->IsDone(); m_pSceneView->Next())
    {
        std::string ulBasePath = "theLights[" + std::to_string(lightIndex) + "].";

        LightComponent* pLight = m_pSceneView->CurrentValue<LightComponent>();
        pLight->SetupLight(shaderID, ulBasePath);
        lightIndex++;
    }

    return true;
}

void LightSystem::Update(double deltatime)
{
    using namespace glm;

    // Set lights to transform position of entity
    for (m_pSceneView->First("light"); !m_pSceneView->IsDone(); m_pSceneView->Next())
    {
        LightComponent* pLight = m_pSceneView->CurrentValue<LightComponent>();
        TransformComponent* pTransform = m_pSceneView->GetComponent<TransformComponent>(pLight->GetEntityID(),
            "transform");


        glm::mat4 rotationMatrix = glm::mat4_cast(pTransform->GetQuatOrientation());
        glm::vec3 rotatedOffset = glm::mat3(rotationMatrix) * pLight->GetPositionOffset();
        glm::vec3 lightPosition = pTransform->GetPosition() + rotatedOffset;

        if (pLight->GetParams().x == 1.0)
        {
            printf("");
        }

        // Light direction temporary just relative to the angle the transform is facing
        // TODO: confirm this is the best way to do this and how it works properly
        glm::vec3 degrees = pTransform->GetOrientation();
        glm::vec3 direction = myutils::GetDirectionFromDegrees(degrees);

        pLight->SetPosition(vec4(lightPosition, 1.0));
        pLight->SetDirection(vec4(direction, 1.0));
    }
}
