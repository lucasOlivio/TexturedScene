#pragma once

#include "Component.h"
#include "EnginePhysics/PhysicsProperties.hpp"
#include <glm/vec3.hpp>
#include <map>

class CollisionComponent : public Component
{
private:
	// TODO: Reestructure this so that we have an interface for shapes
	eShape m_eShape;
	eBodyType m_eBodyType;
	void* m_pShape;

	// If collision is trigered, destroy itself and put a copy of this entity in place
	EntityID m_onColStaticEntity;
	EntityID m_onColKinematicEntity;

	bool m_isActive;

public:
	CollisionComponent();
	virtual ~CollisionComponent();

	bool IsActive();
	void SetActive(bool isActive);

	// Get the shape object alread casted
	template<class T>
	T* GetShape();

	eShape Get_eShape();
	eBodyType Get_eBodyType();

	void Set_eShape(eShape shape);

	// Create the given shape using the parameters
	void SetShape(eShape shape, sParameterInfo& parameterIn);

	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);
};

// TODO: Restructure component and shapes to get rid of this
template<class T>
T* CollisionComponent::GetShape()
{
	return (T*)this->m_pShape;
}