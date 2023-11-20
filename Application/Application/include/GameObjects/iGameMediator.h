#pragma once

#include <glm/vec3.hpp>
#include <string>

class iGameObject;

class iGameMediator
{
public:
	virtual ~iGameMediator() {};

	virtual iGameObject* GetGameObjectByName(std::string tagName) = 0;

	// Get the first target encounter in list direction locked into the 8-way based on the objects position
	// If target not found returns false
	virtual bool GetTargetDirection(std::string targetName, glm::vec3 objPosition, glm::vec3& directionOut) = 0;

	virtual void DestroyGameObj(iGameObject* pGameObj) = 0;

	// Execute an interaction any close entity
	virtual void Interact(std::string tagThatInteracted, glm::vec3 position) = 0;
	virtual void InteractWith(std::string tagToInteract, std::string tagThatInteracted, glm::vec3 position) = 0;

	// Create a new game object from this entity id in this position, rotating it towards the needed direction
	virtual void SpawnGameObj(std::string tagName, glm::vec3 position, glm::vec3 direction) = 0;
	// Random spawn, direction 0
	virtual void SpawnGameObj(std::string tagName) = 0;
};
