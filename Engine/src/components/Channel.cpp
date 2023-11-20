#include "components/Channel.h"
#include "common/utils.h"
#include <glm/gtx/string_cast.hpp>

void ChannelComponent::GetInfo(sComponentInfo& compInfoOut)
{
    using namespace myutils;

    compInfoOut.componentName = "channel";
    compInfoOut.componentParameters.clear();

    this->AddCompParInfo("name", "string", this->m_name, compInfoOut);
    this->AddCompParInfo("volume", "float", this->m_volume, compInfoOut);
    this->AddCompParInfo("pitch", "float", this->m_pitch, compInfoOut);
    this->AddCompParInfo("pan", "float", this->m_pan, compInfoOut);
    this->AddCompParInfo("paused", "bool", this->m_isPaused, compInfoOut);
}

void ChannelComponent::SetParameter(sParameterInfo& parameterIn)
{
    using namespace myutils;

    if (parameterIn.parameterName == "name") {
        this->m_name = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "volume") {
        this->m_volume = parameterIn.parameterFloatValue;
    }
    else if (parameterIn.parameterName == "pitch") {
        this->m_pitch = parameterIn.parameterFloatValue;
    }
    else if (parameterIn.parameterName == "pan") {
        this->m_pan = parameterIn.parameterFloatValue;
    }
    else if (parameterIn.parameterName == "paused") {
        this->m_isPaused = parameterIn.parameterBoolValue;
    }

    return;
}

FMOD::Channel* ChannelComponent::GetChannel()
{
    return this->m_pChannel;
}

void ChannelComponent::SetVolume(float value)
{
    this->m_volume = value;
}

void ChannelComponent::SetPitch(float value)
{
    this->m_pitch = value;
}

void ChannelComponent::SetPan(float value)
{
    this->m_pan = value;
}

void ChannelComponent::SetPaused(bool value)
{
    this->m_isPaused = value;
}

void ChannelComponent::Stop()
{
    this->m_isPaused = true;
}

float ChannelComponent::GetPan()
{
    return this->m_pan;
}
