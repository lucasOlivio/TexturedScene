#include "components/Light.h"
#include "common/utils.h"
#include <string>

// ctors dtors
LightComponent::LightComponent()
{
	m_position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);     // Center of everything
	m_direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);	// Defaults to straight down
	m_positionOffset = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_directionOffset = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White color
	m_specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);     // White LightComponent
	m_atten = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);		// No attenuation
	m_params = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);	    // LightComponent_TYPE POINT
	m_status = false;
	m_distanceOffset = 0.0f;

	m_position_UL = -1;
	m_diffuse_UL = -1;
	m_specular_UL = -1;
	m_atten_UL = -1;
	m_direction_UL = -1;
	m_params_UL = -1;
	m_status_UL = -1;
}

LightComponent::~LightComponent()
{
}

// Setup the UL locations and all LightComponent configurations
void LightComponent::SetupLight(GLint shaderProgramID, std::string ulBasePath)
{
	// Set dot for base path, the LightComponent must be a structure inside the shader so we can set all the parameters
	if (ulBasePath.back() != '.')
	{
		ulBasePath += ".";
	}
	m_ulBasePath = ulBasePath;
	m_position_UL = glGetUniformLocation(shaderProgramID, (m_ulBasePath + "position").c_str());
	m_direction_UL = glGetUniformLocation(shaderProgramID, (m_ulBasePath + "direction").c_str());
	m_diffuse_UL = glGetUniformLocation(shaderProgramID, (m_ulBasePath + "diffuse").c_str());
	m_specular_UL = glGetUniformLocation(shaderProgramID, (m_ulBasePath + "specular").c_str());
	m_atten_UL = glGetUniformLocation(shaderProgramID, (m_ulBasePath + "atten").c_str());
	m_params_UL = glGetUniformLocation(shaderProgramID, (m_ulBasePath + "params").c_str());
	m_status_UL = glGetUniformLocation(shaderProgramID, (m_ulBasePath + "status").c_str());

	// Initial update in uniforms
	SetPosition(m_position);
	SetDirection(m_direction);
	SetDiffuse(m_diffuse);
	SetSpecular(m_specular);
	SetAtten(m_atten);
	SetParams(m_params);
	SetStatus(m_status);
}

// Getters
glm::vec4 LightComponent::GetPosition()
{
	return m_position;
}

glm::vec4 LightComponent::GetDiffuse()
{
	return m_diffuse;
}

glm::vec4 LightComponent::GetSpecular()
{
	return m_specular;
}

glm::vec4 LightComponent::GetAtten()
{
	return m_atten;
}

glm::vec4 LightComponent::GetDirection()
{
	return m_direction;
}

glm::vec4 LightComponent::GetPositionOffset()
{
	return m_positionOffset;
}

glm::vec4 LightComponent::GetDirectionOffset()
{
	return m_directionOffset;
}

glm::vec4 LightComponent::GetParams()
{
	return m_params;
}

bool LightComponent::GetStatus()
{
	return m_status;
}

float LightComponent::GetDistanceOffset()
{
	return m_distanceOffset;
}

// Setters
void LightComponent::SetPosition(glm::vec4 value)
{
	m_position = value;
	m_UpdatePositionUL();
}

void LightComponent::SetDirection(glm::vec4 value)
{
	m_direction = value;
	m_UpdateDirectionUL();
}

void LightComponent::SetPositionOffset(glm::vec4 value)
{
	m_positionOffset = value;
	m_UpdatePositionUL();
}

void LightComponent::SetDirectionOffset(glm::vec4 value)
{
	m_directionOffset = value;
	m_UpdateDirectionUL();
}

void LightComponent::SetDiffuse(glm::vec4 value)
{
	m_diffuse = value;

	glUniform4f(m_diffuse_UL, m_diffuse.r,
		m_diffuse.g, m_diffuse.b,
		m_diffuse.a);
}

void LightComponent::SetSpecular(glm::vec4 value)
{
	m_specular = value;

	glUniform4f(m_specular_UL, m_specular.r,
		m_specular.g, m_specular.b,
		m_specular.a);
}

void LightComponent::SetAtten(glm::vec4 value)
{
	m_atten = value;

	glUniform4f(m_atten_UL, m_atten.x,
		m_atten.y, m_atten.z,
		m_atten.w);
}

void LightComponent::SetParams(glm::vec4 value)
{
	m_params = value;

	glUniform4f(m_params_UL, m_params.x,
		m_params.y, m_params.z,
		m_params.w);
}

void LightComponent::SetStatus(bool vaule)
{
	m_status = vaule;

	glUniform4f(m_status_UL, m_status, 0, 0, 0);
}

void LightComponent::SetLinearAtten(float value)
{
	m_atten[1] = value;
	SetAtten(m_atten);
}

void LightComponent::ResetAtten()
{
	SetAtten(m_initialAtten);
}

void LightComponent::GetInfo(sComponentInfo& compInfoOut)
{
	using namespace myutils;

	compInfoOut.componentName = "light";
	compInfoOut.componentParameters.clear();

	AddCompParInfo("position", "vec4", m_position, compInfoOut);
	AddCompParInfo("direction", "vec4", m_direction, compInfoOut);
	AddCompParInfo("distanceOffset", "float", m_distanceOffset, compInfoOut);
	AddCompParInfo("positionOffset", "vec4", m_positionOffset, compInfoOut);
	AddCompParInfo("directionOffset", "vec4", m_directionOffset, compInfoOut);
	AddCompParInfo("diffuse", "vec4", m_diffuse, compInfoOut);
	AddCompParInfo("specular", "vec4", m_specular, compInfoOut);
	AddCompParInfo("atten", "vec4", m_initialAtten, compInfoOut);
	AddCompParInfo("params", "vec4", m_params, compInfoOut);
	AddCompParInfo("status", "bool", m_status, compInfoOut);
}

void LightComponent::SetParameter(sParameterInfo& parameterIn)
{
	using namespace myutils;

	if (parameterIn.parameterName == "position") {
		SetPosition(parameterIn.parameterVec4Value);
	}
	else if (parameterIn.parameterName == "positionOffset") {
		SetPositionOffset(parameterIn.parameterVec4Value);
	}
	else if (parameterIn.parameterName == "distanceOffset") {
		m_distanceOffset = parameterIn.parameterFloatValue;
	}
	else if (parameterIn.parameterName == "direction") {
		SetDirection(parameterIn.parameterVec4Value);
	}
	else if (parameterIn.parameterName == "directionOffset") {
		SetDirectionOffset(parameterIn.parameterVec4Value);
	}
	else if (parameterIn.parameterName == "diffuse") {
		SetDiffuse(parameterIn.parameterVec4Value);
	}
	else if (parameterIn.parameterName == "specular") {
		SetSpecular(parameterIn.parameterVec4Value);
	}
	else if (parameterIn.parameterName == "atten") {
		SetAtten(parameterIn.parameterVec4Value);
		m_initialAtten = parameterIn.parameterVec4Value;
	}
	else if (parameterIn.parameterName == "params") {
		SetParams(parameterIn.parameterVec4Value);
	}
	else if (parameterIn.parameterName == "status") {
		SetStatus(parameterIn.parameterBoolValue);
	}

	return;
}

void LightComponent::m_UpdatePositionUL()
{
	glm::vec4 newPosition = GetPosition() + GetPositionOffset() + 
						   (GetDirection() * GetDistanceOffset());
	glUniform4f(m_position_UL, newPosition.x,
		newPosition.y, newPosition.z,
		newPosition.w);
}

void LightComponent::m_UpdateDirectionUL()
{
	glm::vec4 newDirection = m_direction + m_directionOffset;
	glUniform4f(m_direction_UL, newDirection.x,
		newDirection.y, newDirection.z,
		newDirection.w);
}