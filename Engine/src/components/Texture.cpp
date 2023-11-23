#include "components/Texture.h"
#include "common/utils.h"

void TextureComponent::GetInfo(sComponentInfo& compInfoOut)
{
    using namespace myutils;

    compInfoOut.componentName = "texture";
    compInfoOut.componentParameters.clear();

    this->AddCompParInfo("fileName", "string", this->fileName, compInfoOut);
    this->AddCompParInfo("textureType", "int", this->textureType, compInfoOut);
}

void TextureComponent::SetParameter(sParameterInfo& parameterIn)
{
    using namespace myutils;

    if (parameterIn.parameterName == "fileName") {
        this->fileName = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "textureType") {
        this->textureType = (eTextureType)parameterIn.parameterIntValue;
    }

    return;
}

