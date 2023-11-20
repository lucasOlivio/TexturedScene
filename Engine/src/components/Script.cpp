#include "components/Script.h"

void ScriptComponent::SetActive(bool isActive)
{
    this->m_isActive = isActive;
}

bool ScriptComponent::IsActive()
{
    return this->m_isActive;
}

std::string ScriptComponent::GetVariable(std::string key)
{
    if (this->m_mapVariables.find(key) == this->m_mapVariables.end())
    {
        return "";
    }
    return this->m_mapVariables[key];
}

void ScriptComponent::SetScriptName(std::string value)
{
    this->m_scriptName = value;
}

std::string ScriptComponent::GetScriptName()
{
    return this->m_scriptName;
}

void ScriptComponent::GetInfo(sComponentInfo& compInfoOut)
{
    compInfoOut.componentName = "script";
    compInfoOut.componentParameters.clear();

    this->AddCompParInfo("scriptName", "string", this->m_scriptName, compInfoOut);
    this->AddCompParInfo("isActive", "bool", this->m_isActive, compInfoOut);
    this->AddCompParInfo("variables", "map", this->m_mapVariables, compInfoOut);
}

void ScriptComponent::SetParameter(sParameterInfo& parameterIn)
{
    if (parameterIn.parameterName == "scriptName") {
        this->m_scriptName = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "isActive") {
        this->m_isActive = parameterIn.parameterBoolValue;
    }
    else if (parameterIn.parameterName == "variables") {
        this->m_mapVariables = parameterIn.parameterMapValue;
    }

    return;
}
