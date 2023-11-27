#pragma once

#include "Component.h"
#include "common/opengl.h"
#include <glm/vec4.hpp>

class LightComponent : public Component
{
public:
	enum LIGHT_TYPE {
		POINT,
		SPOT,
		DIRECTIONAL
	};

	// ctors dtors
	LightComponent();
	virtual ~LightComponent();

	// Getters
	glm::vec4 GetPosition();
	glm::vec4 GetDirection();
	glm::vec4 GetPositionOffset();
	glm::vec4 GetDirectionOffset();
	glm::vec4 GetDiffuse();
	glm::vec4 GetSpecular();
	glm::vec4 GetAtten();
	glm::vec4 GetParams();
	bool GetStatus();
	float GetDistanceOffset();

	// Setters
	void SetPosition(glm::vec4 value);
	void SetDirection(glm::vec4 value);
	void SetPositionOffset(glm::vec4 value);
	void SetDirectionOffset(glm::vec4 value);
	void SetDiffuse(glm::vec4 value);
	void SetSpecular(glm::vec4 value);
	void SetAtten(glm::vec4 value);
	void SetParams(glm::vec4 value);
	void SetStatus(bool value);

	void SetLinearAtten(float value);
	void ResetAtten();

	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);

	// Setup the UL locations and all light configurations
	void SetupLight(GLint shaderProgramID, std::string ulBasePath);

private:
	std::string m_ulBasePath;

	// Controls relative to transform component
	glm::vec4 m_positionOffset;
	glm::vec4 m_directionOffset;
	// multiplied by the direction to put the light at N distance from the front of the object
	float m_distanceOffset; 

	// Light parameters
	glm::vec4 m_position;
	glm::vec4 m_diffuse;   // Colour of the light (used for diffuse)
	glm::vec4 m_specular;  // rgb = highlight colour, w = power
	glm::vec4 m_atten;	 // x = constant, y = linear, z = quadratic, w = DistanceCutOff
	glm::vec4 m_initialAtten;
	glm::vec4 m_direction; // Spot, directional lights
	glm::vec4 m_params;	 // x = Light::LIGHT_TYPE, y = inner angle, z = outer angle, w = TBD
	bool m_status;	     // x = 0 for off, 1 for on

	// Parameters uniform locations
	GLint m_position_UL;
	GLint m_diffuse_UL;
	GLint m_specular_UL;
	GLint m_atten_UL;
	GLint m_direction_UL;
	GLint m_params_UL;
	GLint m_status_UL;

	void m_UpdatePositionUL();
	void m_UpdateDirectionUL();
};