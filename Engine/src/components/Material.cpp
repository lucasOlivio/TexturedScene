#include "components/Material.h"

void MaterialComponent::GetInfo(sComponentInfo& compInfoOut)
{
    compInfoOut.componentName = "material";
    compInfoOut.componentParameters.clear();

    AddCompParInfo("materialName", "string", materialName, compInfoOut);
    AddCompParInfo("colorTextures", "vecStr", colorTextures, compInfoOut);
    AddCompParInfo("colorTexturesRatios", "vec3", colorTexturesRatios, compInfoOut);
    AddCompParInfo("useHeightMap", "bool", useHeightMap, compInfoOut);
    AddCompParInfo("heightMapTexture", "string", heightMapTexture, compInfoOut);
    AddCompParInfo("heightScale", "float", heightScale, compInfoOut);
}

void MaterialComponent::SetParameter(sParameterInfo& parameterIn)
{
    if (parameterIn.parameterName == "materialName") {
        materialName = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "colorTextures") {
        colorTextures = parameterIn.parameterVecStrValue;
    }
    else if (parameterIn.parameterName == "colorTexturesRatios") {
        colorTexturesRatios = parameterIn.parameterVec3Value;
    }
    else if (parameterIn.parameterName == "useHeightMap") {
        useHeightMap = parameterIn.parameterBoolValue;
    }
    else if (parameterIn.parameterName == "heightMapTexture") {
        heightMapTexture = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "heightScale") {
        heightScale = parameterIn.parameterFloatValue;
    }

    return;
}
