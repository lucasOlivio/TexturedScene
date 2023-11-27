#include "EngineRenderer/ShaderManager.h"
#include "common/opengl.h"
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

ShaderManager::Shader::Shader()
{
	ID = 0;
	shaderType = Shader::UNKNOWN;
	return;
}

ShaderManager::Shader::~Shader()
{
	return;
}

std::string ShaderManager::Shader::GetShaderTypeString(void)
{
	switch (shaderType)
	{
	case Shader::VERTEX_SHADER:
		return "VERTEX_SHADER";
		break;
	case Shader::FRAGMENT_SHADER:
		return "FRAGMENT_SHADER";
		break;
	case Shader::UNKNOWN:
	default:
		return "UNKNOWN_SHADER_TYPE";
		break;
	}
	// Should never reach here...
	return "UNKNOWN_SHADER_TYPE";
}

// Look up the uniform inside the shader, then save it, if it finds it
int ShaderManager::ShaderProgram::LoadUniformLocation(const char* variableName)
{
	// 
	int uniLocation = glGetUniformLocation(ID, variableName);
	// Did it find it (not -1)
	if (uniLocation > -1)
	{
		// Save it
		m_mapUniformNameToUniformLocation[variableName] = uniLocation;
	}

	return uniLocation;
}

bool ShaderManager::ShaderProgram::LoadAttributeLocation(const char* variableName)
{
	// 
	GLint attrLocation = glGetAttribLocation(ID, variableName);
	// Did it find it (not -1)
	if (attrLocation == -1)
	{	// Nope.
		return false;
	}
	// Save it
	m_mapAttributeNameToAttributeLocation[variableName] = attrLocation;

	return true;
}

void ShaderManager::ShaderProgram::IsWireframe(bool isWireframe)
{
	if (isWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void ShaderManager::ShaderProgram::SetUniformInt(const char* variableName, int value)
{
	GLint intUL = GetUniformIDFromName(variableName);
	glUniform1i(intUL, value);
}

void ShaderManager::ShaderProgram::SetUniformMatrix4f(const char* variableName, const glm::mat4& value)
{
	GLint matUL = GetUniformIDFromName(variableName);
	glUniformMatrix4fv(matUL, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderManager::ShaderProgram::SetUniformVec4(const char* variableName, const glm::vec4& value)
{
	GLint vecUL = GetUniformIDFromName(variableName);
	glUniform4f(vecUL, value.x, value.y, value.z, value.w);
}

void ShaderManager::ShaderProgram::SetUniformVec2(const char* variableName, const glm::vec2& value)
{
	GLint vecUL = GetUniformIDFromName(variableName);
	glUniform2f(vecUL, value.x, value.y);
}

void ShaderManager::ShaderProgram::BindTexture(uint textureUnit, uint textureId)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void ShaderManager::ShaderProgram::SetUniformFloat(const char* variableName, float value)
{
	GLint floatUL = GetUniformIDFromName(variableName);
	glUniform1f(floatUL, value);
}

ShaderManager::ShaderProgram::ShaderProgram()
{
}

ShaderManager::ShaderProgram::~ShaderProgram()
{
}

// Look up the uniform location and save it.
int ShaderManager::ShaderProgram::GetUniformIDFromName(const char* name)
{
	std::map< std::string /*name of uniform variable*/,
		int /* uniform location ID */ >::iterator
		itUniform = m_mapUniformNameToUniformLocation.find(name);
	if (itUniform == m_mapUniformNameToUniformLocation.end())
	{
		// Not in map yet, so load to map cache
		int uniLocation = LoadUniformLocation(name);
		return uniLocation;
	}

	return itUniform->second;		// second if the "int" value
}

int ShaderManager::ShaderProgram::GetAttributeIDFromName(const char* name)
{
	std::map< std::string /*name of attribute variable*/,
		int /* attribute location ID */ >::iterator
		itAttribute = m_mapAttributeNameToAttributeLocation.find(name);
	if (itAttribute == m_mapAttributeNameToAttributeLocation.end())
	{
		// Not in map yet, so load to map cache
		bool ulFound = LoadAttributeLocation(name);
		if (!ulFound)
		{
			// OpenGL attribute not found value
			return -1;
		}

		itAttribute = m_mapAttributeNameToAttributeLocation.find(name);
	}

	return itAttribute->second;		// second if the "int" value
}
