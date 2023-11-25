#include "EngineRenderer/LightSystem.h"
#include "components/Light.h"
#include "components/Transform.h"

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

        pLight->SetPosition(vec4(pTransform->GetPosition(), 1.0) + pLight->GetDirection());
    }
}
