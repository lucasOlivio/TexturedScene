#include "components/AudioOcclusion.h"
#include "common/utils.h"

void AudioOcclusionComponent::GetInfo(sComponentInfo& compInfoOut)
{
    using namespace myutils;

    compInfoOut.componentName = "audioOcclusion";
    compInfoOut.componentParameters.clear();

    this->AddCompParInfo("direct", "float", this->m_direct, compInfoOut);
    this->AddCompParInfo("reverb", "float", this->m_reverb, compInfoOut);
}

void AudioOcclusionComponent::SetParameter(sParameterInfo& parameterIn)
{
    if (parameterIn.parameterName == "direct") {
        this->m_direct = parameterIn.parameterFloatValue;
    }
    else if (parameterIn.parameterName == "reverb") {
        this->m_reverb = parameterIn.parameterFloatValue;
    }
}

float AudioOcclusionComponent::GetDirect()
{
    return this->m_direct;
}

float AudioOcclusionComponent::GetReverb()
{
    return this->m_reverb;
}
