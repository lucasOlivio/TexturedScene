#include "scene/SceneView.h"
#include "components/Tag.h"

SceneView::SceneView()
{
	m_map.clear();
	m_currIterator = m_map.begin();
}

SceneView::~SceneView()
{
}

void SceneView::First(std::string componentName)
{
	bool hasComponent = GetMapComponents(componentName, m_map);
	if (hasComponent)
	{
		m_currIterator = m_map.begin();
	}
	else
	{
		m_currIterator = m_map.end();
	}
}

void SceneView::Next()
{
	m_currIterator++;
}

void SceneView::End()
{
	m_currIterator = m_map.end();
}

bool SceneView::IsDone()
{
	if (m_currIterator == m_map.end())
	{
		return true;
	}
	return false;
}

EntityID SceneView::CurrentKey()
{
	return m_currIterator->first;
}

int SceneView::GetNumComponents(std::string componentName)
{
	std::map<EntityID, iComponent*> mapComponents;
	GetMapComponents(componentName, mapComponents);
	return (int)mapComponents.size();
}

int SceneView::GetEntityByTag(std::string tagName)
{
	auto tagEntity = m_mapTagEntity.find(tagName);
	if (tagEntity != m_mapTagEntity.end())
	{
		return (int)tagEntity->second;
	}

	// Tag not in cache yet, so add it
	std::map<EntityID, iComponent*> mapTags;
	GetMapComponents("tag", mapTags);
	for (std::pair<EntityID, iComponent*> pairTag : mapTags)
	{
		TagComponent* pTag = (TagComponent*)pairTag.second;
		EntityID currEntity = pairTag.first;

		if (tagName != pTag->name)
		{
			continue;
		}

		m_mapTagEntity[pTag->name] = currEntity;

		return (int)currEntity;
	}

	return -1;
}

iComponent* SceneView::GetComponentByTag(std::string tagName, std::string componentName)
{
	EntityID entityID = GetEntityByTag(tagName);

	return GetComponent(entityID, componentName);
}

iComponent* SceneView::GetComponent(EntityID entityID, std::string componentName)
{
	auto entity = m_map.find(entityID);
	if (entity == m_map.end())
	{
		return nullptr;
	}

	return entity->second;
}
