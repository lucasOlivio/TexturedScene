#include "components/Tag.h"
#include "common/utils.h"
#include <glm/gtx/string_cast.hpp>

void TagComponent::GetInfo(sComponentInfo& compInfoOut)
{
    using namespace myutils;

    compInfoOut.componentName = "tag";
    compInfoOut.componentParameters.clear();

    this->AddCompParInfo("name", "string", this->name, compInfoOut);
}

void TagComponent::SetParameter(sParameterInfo& parameterIn)
{
    using namespace myutils;

    if (parameterIn.parameterName == "name") {
        this->name = parameterIn.parameterStrValue;
    }

    return;
}
