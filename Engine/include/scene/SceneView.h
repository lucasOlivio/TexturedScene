#pragma once

#include "scene/iScene.h"
#include "components/iComponent.h"
#include "common/types.h"
#include <map>

// TODO: Way to make a generic map iterator interface
// Viewer mediator for scene and its components
class SceneView : public iScene
{
private:
	std::map<EntityID, iComponent*> m_map;
	std::map<std::string /*tag name*/, EntityID> m_mapTagEntity;
	std::map<EntityID, iComponent*>::iterator m_currIterator;

public:
	SceneView();
	virtual ~SceneView();

	// TODO: Way to get the component based on the class name, not by a string
	void First(std::string componentName);
	void Next();
	void End();
	bool IsDone();

	EntityID CurrentKey();

	virtual EntityID CreateEntity(EntityID entityID) = 0;

	// Get the component already casted to its class
	template<typename Component>
	Component* CurrentValue()
	{
		iComponent* comp = this->m_currIterator->second;
		assert(comp != nullptr);
		// Only if component found we convert it
		return (Component*)comp;
	}

	virtual int GetNumComponents(std::string componentName);

	// Return the entity id by tag name and saves it to cache
	// Returns -1 if not found
	virtual int GetEntityByTag(std::string tagName);

	virtual iComponent* GetComponent(EntityID entityID, std::string componentName);

	virtual iComponent* GetComponentByTag(std::string tagName, std::string componentName);

	// Get a specific component from an entity on the scene
	template<typename Component>
	Component* GetComponent(EntityID entityID, std::string componentName)
	{
		iComponent* comp = this->GetComponent(entityID, componentName);
		assert(comp != nullptr);
		// Only if component found we convert it
		return (Component*)comp;
	}

	template<typename Component>
	Component* GetComponentByTag(std::string tagName, std::string componentName)
	{
		iComponent* comp = this->GetComponentByTag(tagName, componentName);
		assert(comp != nullptr);
		// Only if component found we convert it
		return (Component*)comp;
	}
};