#include "components/Material.h"

void MaterialComponent::GetInfo(sComponentInfo& compInfoOut)
{
    compInfoOut.componentName = "material";
    compInfoOut.componentParameters.clear();

    AddCompParInfo("materialName", "string", materialName, compInfoOut);
    AddCompParInfo("offsetMove", "vec3", offsetMove, compInfoOut);
    AddCompParInfo("colorTextures", "vecStr", colorTextures, compInfoOut);
    AddCompParInfo("colorTexturesRatios", "vec3", colorTexturesRatios, compInfoOut);
    AddCompParInfo("useHeightMap", "bool", useHeightMap, compInfoOut);
    AddCompParInfo("heightMapTexture", "string", heightMapTexture, compInfoOut);
    AddCompParInfo("heightScale", "float", heightScale, compInfoOut);
    AddCompParInfo("useNormalTexture", "bool", useNormalTexture, compInfoOut);
    AddCompParInfo("normalTexture", "string", normalTexture, compInfoOut);
    AddCompParInfo("useSpecularTexture", "bool", useSpecularTexture, compInfoOut);
    AddCompParInfo("specularTexture", "string", specularTexture, compInfoOut);
    AddCompParInfo("useDiscardTexture", "bool", useDiscardTexture, compInfoOut);
    AddCompParInfo("discardTexture", "string", discardTexture, compInfoOut);
    AddCompParInfo("useCubeTexture", "bool", useCubeTexture, compInfoOut);
    AddCompParInfo("cubeTexture", "string", cubeTexture, compInfoOut);
    AddCompParInfo("alphaValue", "float", alphaValue, compInfoOut);
}

void MaterialComponent::SetParameter(sParameterInfo& parameterIn)
{
    if (parameterIn.parameterName == "materialName") {
        materialName = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "offsetMove") {
        offsetMove = parameterIn.parameterVec3Value;
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
    else if (parameterIn.parameterName == "useNormalTexture") {
        useNormalTexture = parameterIn.parameterBoolValue;
    }
    else if (parameterIn.parameterName == "normalTexture") {
        normalTexture = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "useSpecularTexture") {
        useSpecularTexture = parameterIn.parameterBoolValue;
    }
    else if (parameterIn.parameterName == "specularTexture") {
        specularTexture = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "useDiscardTexture") {
        useDiscardTexture = parameterIn.parameterBoolValue;
    }
    else if (parameterIn.parameterName == "discardTexture") {
        discardTexture = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "useCubeTexture") {
        useCubeTexture = parameterIn.parameterBoolValue;
    }
    else if (parameterIn.parameterName == "cubeTexture") {
        cubeTexture = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "alphaValue") {
        alphaValue = parameterIn.parameterFloatValue;
    }

    return;
}
