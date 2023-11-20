#pragma once

#include "Component.h"
#include <map>

class ScriptComponent : public Component
{
	std::string m_scriptName;
	bool m_isActive;

	std::map<std::string, std::string> m_mapVariables;
public:
	void SetActive(bool isActive);
	bool IsActive();

	// Get the given variable from the map, if variable not found them returns empty string
	std::string GetVariable(std::string key);

	void SetScriptName(std::string value);
	std::string GetScriptName();

	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);
};
