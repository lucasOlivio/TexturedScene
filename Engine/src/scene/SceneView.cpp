#include "scene/SceneView.h"
#include "components/Tag.h"

SceneView::SceneView()
{
	this->m_map.clear();
	this->m_currIterator = this->m_map.begin();
}

SceneView::~SceneView()
{
}

void SceneView::First(std::string componentName)
{
	bool hasComponent = this->GetMapComponents(componentName, this->m_map);
	if (hasComponent)
	{
		this->m_currIterator = this->m_map.begin();
	}
	else
	{
		this->m_currIterator = this->m_map.end();
	}
}

void SceneView::Next()
{
	this->m_currIterator++;
}

void SceneView::End()
{
	this->m_currIterator = this->m_map.end();
}

bool SceneView::IsDone()
{
	if (this->m_currIterator == this->m_map.end())
	{
		return true;
	}
	return false;
}

EntityID SceneView::CurrentKey()
{
	return this->m_currIterator->first;
}

int SceneView::GetNumComponents(std::string componentName)
{
	std::map<EntityID, iComponent*> mapComponents;
	this->GetMapComponents(componentName, mapComponents);
	return (int)mapComponents.size();
}

int SceneView::GetEntityByTag(std::string tagName)
{
	auto tagEntity = this->m_mapTagEntity.find(tagName);
	if (tagEntity != this->m_mapTagEntity.end())
	{
		return (int)tagEntity->second;
	}

	// Tag not in cache yet, so add it
	for (this->First("tag"); !this->IsDone(); this->Next())
	{
		TagComponent* pTag = this->CurrentValue<TagComponent>();
		if (tagName != pTag->name)
		{
			continue;
		}

		EntityID currEntity = this->CurrentKey();

		this->m_mapTagEntity[pTag->name] = currEntity;

		return (int)currEntity;
	}

	return -1;
}

iComponent* SceneView::GetComponentByTag(std::string tagName, std::string componentName)
{
	EntityID entityID = this->GetEntityByTag(tagName);

	return this->GetComponent(entityID, componentName);
}

iComponent* SceneView::GetComponent(EntityID entityID, std::string componentName)
{
	auto entity = this->m_map.find(entityID);
	if (entity == this->m_map.end())
	{
		return nullptr;
	}

	return entity->second;
}
