#include "EngineRenderer/LightSystem.h"
#include "components/Light.h"

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
