#include "scene/Scene.h"
#include "components/Model.h"
#include "components/Tag.h"
#include "components/ComponentBuilder.h"

Scene::Scene(iEvent* pKeyEvents, iEvent* pCollisionEvents)
{
    this->m_isPlaying = false;
    this->m_pKeyEvents = pKeyEvents;
    this->m_pCollisionEvents = pCollisionEvents;
    this->m_numEntities = 0;
}

Scene::~Scene()
{
    this->m_components.clear();
}

void Scene::Clear()
{
    for (auto& pairComponent : this->m_components) {
        for (auto& pairEntityComp : pairComponent.second) {
            iComponent* pComp = pairEntityComp.second;
            pComp->SetDeleted(true);
            this->m_compToDestroy.push_back(pairEntityComp.second);
        }
        pairComponent.second.clear();
    }
    this->m_components.clear();
    this->m_numEntities = 0;
}

void Scene::ClearDeleted()
{
    for (iComponent* pComp : this->m_compToDestroy)
    {
        delete pComp;
    }
    this->m_compToDestroy.clear();
}

void Scene::ClearListenerDeleted()
{
    for (iListener* pListener : this->m_listenerToDestroy)
    {
        this->m_pCollisionEvents->Dettach(pListener);
    }
    this->m_listenerToDestroy.clear();
}

EntityID Scene::GetNumEntities()
{
    return this->m_numEntities;
}

EntityID Scene::CreateEntity()
{
    EntityID newEntityID = this->m_numEntities;
    this->m_numEntities++;
    return newEntityID;
}

EntityID Scene::CreateEntity(EntityID entityID)
{
    EntityID newEntityID = this->CreateEntity();
    
    std::vector<sComponentInfo> componentsInfo = this->GetComponentsInfo(entityID);

    // Go for each component and copy it to the new one
    ComponentBuilder compBuilder = ComponentBuilder(this);
    for (sComponentInfo sCompInfo : componentsInfo)
    {
        iComponent* pNewComp = compBuilder.BuildComponent(sCompInfo, newEntityID);

        // Copy meshes info for models and animations too
        if (sCompInfo.componentName == "model")
        {
            ModelComponent* pNewModelComp = (ModelComponent*)pNewComp;
            ModelComponent* pModelComp = (ModelComponent*)this->GetComponent(entityID, "model");
            pNewModelComp->SetMeshes(pModelComp->GetMeshes());
        }
    }

    return newEntityID;
}

void Scene::DeleteEntity(EntityID entityID)
{
    for (auto& pairComponent : this->m_components) {
        auto& innerMap = pairComponent.second;
        auto entityIter = innerMap.find(entityID);
        if (entityIter != innerMap.end()) {
            iComponent* pComp = entityIter->second;
            pComp->SetDeleted(true);
            this->m_compToDestroy.push_back(pComp);
            innerMap.erase(entityIter);
        }
    }
}

void Scene::DeleteListener(iListener* pListener)
{
    this->m_listenerToDestroy.push_back(pListener);
}

bool Scene::GetMapComponents(std::string componentName, std::map<EntityID, iComponent*>& componentsOut)
{
    std::map<std::string,
        std::map<EntityID, iComponent*>>::iterator it;
    it = this->m_components.find(componentName);

    if (it == this->m_components.end())
    {
        // No components of this type
        return false;
    }

    componentsOut = it->second;
    return true;
}

EntityID Scene::GetEntity(std::string tagName)
{
    for (this->First("tag"); !this->IsDone(); this->Next())
    {
        TagComponent* pTag = this->CurrentValue<TagComponent>();

        if (pTag->name == tagName)
        {
            return this->CurrentKey();
        }
    }
    assert(false); // Error if not found. TODO: Better way to raise error here
    return 0;
}

iComponent* Scene::GetComponent(EntityID entityID, std::string componentName)
{
    std::map<EntityID, iComponent*> mapComponents;

    this->GetMapComponents(componentName, mapComponents);

    std::map<EntityID, iComponent*>::iterator it;
    it = mapComponents.find(entityID);

    if (it == mapComponents.end())
    {
        // Entity doesn't have this kind of component
        return nullptr;
    }

    return it->second;
}

void Scene::SetComponent(EntityID entityID, std::string componentName, iComponent* componentIn)
{
    // Check the entity already have this component
    iComponent* compToDelete = this->GetComponent(entityID, componentName);

    // If already have, delete it first
    if (compToDelete)
    {
        compToDelete->SetDeleted(true);
        this->m_compToDestroy.push_back(compToDelete);
    }

    // Now replace with the new component
    this->m_components[componentName][entityID] = componentIn;

    return;
}

std::vector<sComponentInfo> Scene::GetComponentsInfo(EntityID entityID)
{
    std::vector<sComponentInfo> componentsInfo;

    // Go through each component type in the scene
    for (const auto& pairComponents : this->m_components)
    {
        // Check if user has this component
        const auto& itComponents = pairComponents.second.find(entityID);
        if (itComponents == pairComponents.second.end())
        {
            continue;
        }

        // Get info from component and attach to vector
        sComponentInfo componentInfo;
        itComponents->second->GetInfo(componentInfo);

        componentsInfo.push_back(componentInfo);
    }

    return componentsInfo;
}

std::map<std::string, iComponent*> Scene::GetComponents(EntityID entityID)
{
    std::map<std::string, iComponent*> components;

    // Go through each component type in the scene
    for (const auto& pairComponents : this->m_components)
    {
        // Check if user has this component
        const auto& itComponents = pairComponents.second.find(entityID);
        if (itComponents == pairComponents.second.end())
        {
            continue;
        }

        // Get info from component and attach to vector
        components[pairComponents.first] = itComponents->second;
    }

    return components;
}

bool Scene::IsPlaying()
{
    if (this->m_isPlaying)
    {
        return true;
    } 
    else
    {
        return false;
    }
}

void Scene::SetPlaying(bool isPlaying)
{
    this->m_isPlaying = isPlaying;
}
