#include "components/Component.h"
#include "common/utils.h"

const int FLOAT_DECIMAL_PRECISION = 5;

Component::Component()
{
    this->m_isDeleted = false;
}

void Component::SetDeleted(bool isDeleted)
{
    this->m_isDeleted = isDeleted;
}

bool Component::IsDeleted()
{
    if (this->m_isDeleted)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// TODO: Is there a better way to do this?
void Component::AddCompParInfo(const std::string& name,
                               const std::string& type,
                               const std::string& value,
                               sComponentInfo& compInfoOut)
{
    sParameterInfo paramInfo;
    paramInfo.parameterName = name;
    paramInfo.parameterType = type;
    paramInfo.parameterStrValue = value;
    compInfoOut.componentParameters.push_back(paramInfo);

    return;
}

void Component::AddCompParInfo(const std::string& name,
                               const std::string& type,
                               const int& value,
                               sComponentInfo& compInfoOut)
{
    sParameterInfo paramInfo;
    paramInfo.parameterName = name;
    paramInfo.parameterType = type;
    paramInfo.parameterIntValue = value;
    compInfoOut.componentParameters.push_back(paramInfo);

    return;
}

void Component::AddCompParInfo(const std::string& name,
                               const std::string& type,
                               const bool& value,
                               sComponentInfo& compInfoOut)
{
    sParameterInfo paramInfo;
    paramInfo.parameterName = name;
    paramInfo.parameterType = type;
    paramInfo.parameterBoolValue = value;
    compInfoOut.componentParameters.push_back(paramInfo);

    return;
}

void Component::AddCompParInfo(const std::string& name,
                               const std::string& type,
                               const float& value,
                               sComponentInfo& compInfoOut)
{
    using namespace myutils;

    sParameterInfo paramInfo;
    paramInfo.parameterName = name;
    paramInfo.parameterType = type;
    paramInfo.parameterFloatValue = (float)FormatFloatWithPrecision(value, FLOAT_DECIMAL_PRECISION);
    compInfoOut.componentParameters.push_back(paramInfo);

    return;
}

void Component::AddCompParInfo(const std::string& name,
                               const std::string& type,
                               const glm::vec3& value,
                               sComponentInfo& compInfoOut)
{
    using namespace myutils;

    sParameterInfo paramInfo;
    paramInfo.parameterName = name;
    paramInfo.parameterType = type;
    paramInfo.parameterVec3Value.x = (float)FormatFloatWithPrecision(value.x, FLOAT_DECIMAL_PRECISION);
    paramInfo.parameterVec3Value.y = (float)FormatFloatWithPrecision(value.y, FLOAT_DECIMAL_PRECISION);
    paramInfo.parameterVec3Value.z = (float)FormatFloatWithPrecision(value.z, FLOAT_DECIMAL_PRECISION);
    compInfoOut.componentParameters.push_back(paramInfo);

    return;
}

void Component::AddCompParInfo(const std::string& name,
                               const std::string& type,
                               const glm::vec4& value,
                               sComponentInfo& compInfoOut)
{
    using namespace myutils;

    sParameterInfo paramInfo;
    paramInfo.parameterName = name;
    paramInfo.parameterType = type;
    paramInfo.parameterVec4Value.x = (float)FormatFloatWithPrecision(value.x, FLOAT_DECIMAL_PRECISION);
    paramInfo.parameterVec4Value.y = (float)FormatFloatWithPrecision(value.y, FLOAT_DECIMAL_PRECISION);
    paramInfo.parameterVec4Value.z = (float)FormatFloatWithPrecision(value.z, FLOAT_DECIMAL_PRECISION);
    paramInfo.parameterVec4Value.w = (float)FormatFloatWithPrecision(value.w, FLOAT_DECIMAL_PRECISION);
    compInfoOut.componentParameters.push_back(paramInfo);

    return;
}

void Component::AddCompParInfo(const std::string& name,
                                const std::string& type,
                                const std::vector<std::string>& value,
                                sComponentInfo& compInfoOut)
{
    sParameterInfo paramInfo;
    paramInfo.parameterName = name;
    paramInfo.parameterType = type;
    paramInfo.parameterVecStrValue = value;
    compInfoOut.componentParameters.push_back(paramInfo);

    return;
}

void Component::AddCompParInfo(const std::string& name,
    const std::string& type,
    const std::vector<float>& value,
    sComponentInfo& compInfoOut)
{
    sParameterInfo paramInfo;
    paramInfo.parameterName = name;
    paramInfo.parameterType = type;
    paramInfo.parameterVecFloatValue = value;
    compInfoOut.componentParameters.push_back(paramInfo);

    return;
}

void Component::AddCompParInfo(const std::string& name, const std::string& type, const std::map<std::string, std::string>& value, sComponentInfo& compInfoOut)
{
    sParameterInfo paramInfo;
    paramInfo.parameterName = name;
    paramInfo.parameterType = type;
    paramInfo.parameterMapValue = value;
    compInfoOut.componentParameters.push_back(paramInfo);

    return;
}

EntityID Component::GetEntityID()
{
    return this->m_entityID;
}

void Component::SetEntityID(EntityID entityID)
{
    this->m_entityID = entityID;
}
