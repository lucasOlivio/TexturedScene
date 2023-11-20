#pragma once

#include "Component.h"

class ForceComponent : public Component
{
private:
	bool m_isActive;
	float m_inverseMass;
	float m_restitution;
	glm::vec3 m_acceleration;
	glm::vec3 m_velocity;

	// Change acceleration towards a point
	glm::vec3 m_adjustTowards;
	glm::vec3 m_adjustValue;
public:
	ForceComponent();
	~ForceComponent();

	// Setters
	void SetInverseMass(float value);
	void SetRestitution(float value);
	void SetAcceleration(glm::vec3 value);
	void SetVelocity(glm::vec3 value);
	void SetActive(bool isActive);
	void SetAdjustTowards(glm::vec3 value);
	void SetAdjustValue(glm::vec3 value);

	// Getters
	float GetInverseMass();
	float GetRestitution();
	glm::vec3 GetAcceleration();
	glm::vec3 GetVelocity();
	bool IsActive();
	glm::vec3 GetAdjustTowards();
	glm::vec3 GetAdjustValue();

	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);
};

