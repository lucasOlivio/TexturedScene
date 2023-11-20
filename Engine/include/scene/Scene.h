#pragma once

#include "scene/SceneView.h"
#include "components/iComponent.h"
#include "events/iEvent.h"
#include "common/types.h"
#include <map>
#include <string>

class Scene : public SceneView
{
private:
	EntityID m_numEntities;

	// TODO: For now keep as a map for simplicity, 
	// but later transform into a vector of entities
	// and each entity with a bitmask of components
	std::map<std::string /* component name */,
			 std::map<EntityID, iComponent*>> m_components;

	bool m_isPlaying;

	iEvent* m_pKeyEvents;
	iEvent* m_pCollisionEvents;

	// Components and listeners that were already deleted and are waiting end of frame to 
	// be completely removed
	std::vector<iComponent*> m_compToDestroy;
	std::vector<iListener*> m_listenerToDestroy;

public:
	Scene(iEvent* pKeyEvents, iEvent* pCollisionEvents);
	~Scene();

	// Delete all components pointers and clear maps
	virtual void Clear();

	// Delete components that were marked
	void ClearDeleted();
	// Delete listeners that were marked
	void ClearListenerDeleted();

	virtual EntityID GetNumEntities();

	virtual EntityID CreateEntity();
	// Create a new entity copy of the "entityID" passed
	virtual EntityID CreateEntity(EntityID entityID);

	virtual void DeleteEntity(EntityID entityID);
	virtual void DeleteListener(iListener* pListener);

	// Get all parameters info from all the components
	// this entity has attached to it
	virtual std::vector<sComponentInfo> GetComponentsInfo(EntityID entityID);
	virtual std::map<std::string, iComponent*> GetComponents(EntityID entityID);

	// Get the vector for the respective component
	virtual bool GetMapComponents(std::string componentName, std::map<EntityID, iComponent*>& componentsOut);

	// Get an entity from the scene by its tag name
	virtual EntityID GetEntity(std::string tagName);
	// Get a component from the scene by its entity id and component name
	virtual iComponent* GetComponent(EntityID entityID, std::string componentName);
	// Set a component to the entity by its entity id and component name
	virtual void SetComponent(EntityID entityID, std::string componentName, iComponent* componentIn);

	virtual bool IsPlaying();
	void SetPlaying(bool isPlaying);
};