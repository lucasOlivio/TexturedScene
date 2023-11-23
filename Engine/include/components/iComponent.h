#pragma once

#include "common/types.h"
#include <map>
#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// Parsed info to be used decoupled
struct sParameterInfo
{
	std::string parameterName;
	std::string parameterType;
	// TODO: Is there a better way to do this? 
	// Maybe with bitmaps as the ECS, but for parameters too, or it would be too much?
	std::string parameterStrValue;
	int parameterIntValue;
	bool parameterBoolValue;
	float parameterFloatValue;
	glm::vec3 parameterVec3Value;
	glm::vec4 parameterVec4Value;
	std::vector<float> parameterVecFloatValue;
	std::vector<std::string> parameterVecStrValue;
	std::map<std::string, std::string> parameterMapValue;
};

struct sComponentInfo
{
	std::string componentName;
	std::vector<sParameterInfo> componentParameters;
};

class iComponent
{
public:
	virtual ~iComponent() {};

	virtual EntityID GetEntityID() = 0;
	virtual void SetEntityID(EntityID entityID) = 0;

	virtual void SetDeleted(bool isDeleted) = 0;
	virtual bool IsDeleted() = 0;

	virtual void GetInfo(sComponentInfo& compInfoOut) = 0;
	virtual void SetParameter(sParameterInfo& parameterIn) = 0;
};
