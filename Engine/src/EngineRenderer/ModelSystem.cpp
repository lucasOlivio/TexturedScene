#include "EngineRenderer/ModelSystem.h"
#include "components/iComponent.h"
#include "components/Model.h"
#include "components/Transform.h"
#include "common/opengl.h"
#include "common/utilsMat.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

ModelSystem::ModelSystem(iShaderInfo* pShaderInfo, SceneView* pSceneView)
{
    this->m_pShaderInfo = pShaderInfo;
    this->m_pSceneView = pSceneView;
    this->m_pVAOManager = new VAOManager(this->m_pShaderInfo);
}

ModelSystem::~ModelSystem()
{
    delete this->m_pVAOManager;
}

bool ModelSystem::LoadModels(int shaderID)
{
    for (this->m_pSceneView->First("model"); !this->m_pSceneView->IsDone(); this->m_pSceneView->Next())
    {
        ModelComponent* pModel = this->m_pSceneView->CurrentValue<ModelComponent>();
        // For now only debug objects will need to be dynamic
        for (int i = 0; i < pModel->models.size(); i++)
        {
            sMesh* pMesh = this->m_pVAOManager->LoadModelIntoVAO(pModel->models[i], shaderID, false);
            if (!pMesh)
            {
                return false;
            }
            pModel->SetMesh(i, pMesh);
        }

        if (pModel->collisionName == "")
        {
            continue;
        }

        pModel->pCollisionMesh = this->m_pVAOManager->LoadModelIntoVAO(pModel->collisionName, shaderID, false);
        if (!pModel->pCollisionMesh)
        {
            return false;
        }
    }

    return true;
}

bool ModelSystem::LoadModels(std::string basePath, int shaderID)
{
    printf("Loading models...\n");
    this->m_pVAOManager->SetBasePath(basePath);

    bool isLoaded = this->LoadModels(shaderID);
    if (isLoaded)
    {
        return true;
    }
    return false;
}

void ModelSystem::Destroy()
{
    for (this->m_pSceneView->First("model"); !this->m_pSceneView->IsDone(); this->m_pSceneView->Next())
    {
        ModelComponent* pModel = this->m_pSceneView->CurrentValue<ModelComponent>();
        
        for (std::string model : pModel->models)
        {
            this->m_pVAOManager->DestroyVBO(model);
        }
    }
}

