#include "components/Model.h"
#include "common/utils.h"
#include "common/opengl.h"
#include "common/constants.h"

ModelComponent::ModelComponent()
{
    this->m_currFrame = 0;
    this->m_elapsedTime = 0;
    this->m_pMeshes.clear();
}

void ModelComponent::SetMeshes(std::vector<sMesh*> meshes)
{
    this->m_pMeshes = meshes;
}

void ModelComponent::SetMesh(uint index, sMesh* pMesh)
{
    if (index > this->m_pMeshes.size())
    {
        return;
    }

    this->m_pMeshes[index] = pMesh;
}

std::vector<sMesh*> ModelComponent::GetMeshes()
{
    return this->m_pMeshes;
}

void ModelComponent::AddMesh(sMesh* pMesh)
{
    this->m_pMeshes.push_back(pMesh);
}

sMesh* ModelComponent::GetMesh(uint index)
{
    assert(index < this->m_pMeshes.size());
    return this->m_pMeshes[index];
}

sMesh* ModelComponent::GetCurrentMesh()
{
    return this->GetMesh(this->m_currFrame);
}

void ModelComponent::GetInfo(sComponentInfo& compInfoOut)
{
	using namespace myutils;

	compInfoOut.componentName = "model";
	compInfoOut.componentParameters.clear();

    this->AddCompParInfo("models", "vecStr", this->models, compInfoOut);
    this->AddCompParInfo("collisionName", "string", this->collisionName, compInfoOut);
    this->AddCompParInfo("material", "string", this->material, compInfoOut);
	this->AddCompParInfo("friendlyName", "string", this->friendlyName, compInfoOut);
    this->AddCompParInfo("parentTagName", "string", this->parentTagName, compInfoOut);
	this->AddCompParInfo("isWireframe", "bool", this->isWireframe, compInfoOut);
	this->AddCompParInfo("doNotLight", "bool", this->doNotLight, compInfoOut);
    this->AddCompParInfo("useColorTexture", "bool", this->useColorTexture, compInfoOut);
    this->AddCompParInfo("isActive", "bool", this->m_isActive, compInfoOut);
}

void ModelComponent::SetParameter(sParameterInfo& parameterIn)
{
    using namespace myutils;

    if (parameterIn.parameterName == "models") {
        this->m_currFrame = 0;
        this->models = parameterIn.parameterVecStrValue;
        this->m_pMeshes.resize(this->models.size(), nullptr);
    }
    else if (parameterIn.parameterName == "material") {
        this->material = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "collisionName") {
        this->collisionName = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "friendlyName") {
        this->friendlyName = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "parentTagName") {
        this->parentTagName = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "isWireframe") {
        this->isWireframe = parameterIn.parameterBoolValue;
    }
    else if (parameterIn.parameterName == "doNotLight") {
        this->doNotLight = parameterIn.parameterBoolValue;
    }
    else if (parameterIn.parameterName == "useColorTexture") {
        this->useColorTexture = parameterIn.parameterBoolValue;
    }
    else if (parameterIn.parameterName == "isActive") {
        this->m_isActive = parameterIn.parameterBoolValue;
    }

    return;
}


void ModelComponent::Update(double deltaTime)
{
    if (!this->m_isActive)
    {
        return;
    }

    this->m_UpdateFrames(deltaTime);

}

void ModelComponent::SetActive(bool isActive)
{
    this->m_isActive = isActive;
}

bool ModelComponent::IsActive()
{
    if (this->m_isActive)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void ModelComponent::m_UpdateFrames(double deltaTime)
{
    this->m_elapsedTime += deltaTime;

    if (this->m_elapsedTime < FRAME_DURATION)
    {
        return;
    }

    // Update the animation index
    this->m_currFrame++;
    if (this->m_currFrame >= this->m_pMeshes.size()) {
        this->m_currFrame = 0;  // Loop the animation if needed
    }

    // Reset the elapsed time
    this->m_elapsedTime = 0.0;
}
