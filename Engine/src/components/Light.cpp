#include "components/Light.h"
#include "common/utils.h"
#include <string>

// ctors dtors
LightComponent::LightComponent()
{
	position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);     // Center of everything
	diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);		// White color
	specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);     // White LightComponent
	atten = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);		// No attenuation
	direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);	// Defaults to straight down
	params = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);	    // LightComponent_TYPE POINT
	status = false;	                                // Off

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
	m_diffuse_UL = glGetUniformLocation(shaderProgramID, (m_ulBasePath + "diffuse").c_str());
	m_specular_UL = glGetUniformLocation(shaderProgramID, (m_ulBasePath + "specular").c_str());
	m_atten_UL = glGetUniformLocation(shaderProgramID, (m_ulBasePath + "atten").c_str());
	m_direction_UL = glGetUniformLocation(shaderProgramID, (m_ulBasePath + "direction").c_str());
	m_params_UL = glGetUniformLocation(shaderProgramID, (m_ulBasePath + "params").c_str());
	m_status_UL = glGetUniformLocation(shaderProgramID, (m_ulBasePath + "status").c_str());

	glUniform4f(m_position_UL, position.x,
		position.y, position.z,
		position.w);
	glUniform4f(m_diffuse_UL, diffuse.r,
		diffuse.g, diffuse.b,
		diffuse.a);
	glUniform4f(m_specular_UL, specular.r,
		specular.g, specular.b,
		specular.a);
	glUniform4f(m_atten_UL, atten.x,
		atten.y, atten.z,
		atten.w);
	glUniform4f(m_direction_UL, direction.x,
		direction.y, direction.z,
		direction.w);
	glUniform4f(m_direction_UL, direction.x,
		direction.y, direction.z,
		direction.w);
	glUniform4f(m_params_UL, params.x,
		params.y, params.z,
		params.w);
	glUniform4f(m_status_UL, status, 0, 0, 0);
}

// Getters
glm::vec4 LightComponent::GetPosition()
{
	return position;
}

glm::vec4 LightComponent::GetDiffuse()
{
	return diffuse;
}

glm::vec4 LightComponent::GetSpecular()
{
	return specular;
}

glm::vec4 LightComponent::GetAtten()
{
	return atten;
}

glm::vec4 LightComponent::GetDirection()
{
	return direction;
}

glm::vec4 LightComponent::GetParams()
{
	return params;
}

bool LightComponent::GetStatus()
{
	return status;
}

// Setters
void LightComponent::SetPosition(glm::vec4 newPosition)
{
	position = newPosition;

	if (m_position_UL == -1)
	{
		return;
	}

	glUniform4f(m_position_UL, position.x,
		position.y, position.z,
		position.w);
}

void LightComponent::SetDiffuse(glm::vec4 newDiffuse)
{
	diffuse = newDiffuse;

	if (m_diffuse_UL == -1)
	{
		return;
	}

	glUniform4f(m_diffuse_UL, diffuse.r,
		diffuse.g, diffuse.b,
		diffuse.a);
}

void LightComponent::SetSpecular(glm::vec4 newSpecular)
{
	specular = newSpecular;

	if (m_specular_UL == -1)
	{
		return;
	}

	glUniform4f(m_specular_UL, specular.r,
		specular.g, specular.b,
		specular.a);
}

void LightComponent::SetAtten(glm::vec4 newAtten)
{
	atten = newAtten;

	if (m_atten_UL == -1)
	{
		return;
	}

	glUniform4f(m_atten_UL, atten.x,
		atten.y, atten.z,
		atten.w);
}

void LightComponent::SetDirection(glm::vec4 newDirection)
{
	direction = newDirection;

	if (m_direction_UL == -1)
	{
		return;
	}

	glUniform4f(m_direction_UL, direction.x,
		direction.y, direction.z,
		direction.w);
}

void LightComponent::SetParams(glm::vec4 newParams)
{
	params = newParams;

	if (m_params_UL == -1)
	{
		return;
	}

	glUniform4f(m_params_UL, params.x,
		params.y, params.z,
		params.w);
}

void LightComponent::SetStatus(bool newStatus)
{
	status = newStatus;

	if (m_status_UL == -1)
	{
		return;
	}

	glUniform4f(m_status_UL, status, 0, 0, 0);
}

void LightComponent::SetLinearAtten(float value)
{
	atten[1] = value;
	SetAtten(atten);
}

void LightComponent::ResetAtten()
{
	SetAtten(initialAtten);
}

void LightComponent::GetInfo(sComponentInfo& compInfoOut)
{
	using namespace myutils;

	compInfoOut.componentName = "light";
	compInfoOut.componentParameters.clear();

	AddCompParInfo("position", "vec4", position, compInfoOut);
	AddCompParInfo("diffuse", "vec4", diffuse, compInfoOut);
	AddCompParInfo("specular", "vec4", specular, compInfoOut);
	AddCompParInfo("atten", "vec4", initialAtten, compInfoOut);
	AddCompParInfo("direction", "vec4", direction, compInfoOut);
	AddCompParInfo("params", "vec4", params, compInfoOut);
	AddCompParInfo("status", "bool", status, compInfoOut);
}

void LightComponent::SetParameter(sParameterInfo& parameterIn)
{
	using namespace myutils;

	if (parameterIn.parameterName == "position") {
		SetPosition(parameterIn.parameterVec4Value);
	}
	else if (parameterIn.parameterName == "diffuse") {
		SetDiffuse(parameterIn.parameterVec4Value);
	}
	else if (parameterIn.parameterName == "specular") {
		SetSpecular(parameterIn.parameterVec4Value);
	}
	else if (parameterIn.parameterName == "atten") {
		SetAtten(parameterIn.parameterVec4Value);
		initialAtten = parameterIn.parameterVec4Value;
	}
	else if (parameterIn.parameterName == "direction") {
		SetDirection(parameterIn.parameterVec4Value);
	}
	else if (parameterIn.parameterName == "params") {
		SetParams(parameterIn.parameterVec4Value);
	}
	else if (parameterIn.parameterName == "status") {
		SetStatus(parameterIn.parameterBoolValue);
	}

	return;
}

