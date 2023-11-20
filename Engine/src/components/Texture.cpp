#include "components/Texture.h"
#include "common/utils.h"

void TextureComponent::GetInfo(sComponentInfo& compInfoOut)
{
    using namespace myutils;

    compInfoOut.componentName = "texture";
    compInfoOut.componentParameters.clear();

    this->AddCompParInfo("fileName", "string", this->fileName, compInfoOut);
    this->AddCompParInfo("samplerId", "int", this->samplerId, compInfoOut);
    this->AddCompParInfo("generateMIPMap", "bool", this->generateMIPMap, compInfoOut);
}

void TextureComponent::SetParameter(sParameterInfo& parameterIn)
{
    using namespace myutils;

    if (parameterIn.parameterName == "fileName") {
        this->fileName = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "samplerId") {
        this->samplerId = parameterIn.parameterIntValue;
    }
    else if (parameterIn.parameterName == "generateMIPMap") {
        this->generateMIPMap = parameterIn.parameterBoolValue;
    }

    return;
}

