#include "components/Audio.h"
#include "common/utils.h"

void AudioComponent::GetInfo(sComponentInfo& compInfoOut)
{
    using namespace myutils;

    compInfoOut.componentName = "audio";
    compInfoOut.componentParameters.clear();

    this->AddCompParInfo("name", "string", this->m_name, compInfoOut);
    this->AddCompParInfo("idChannelGroup", "int", this->m_idChannelGroup, compInfoOut);
    this->AddCompParInfo("isStream", "bool", this->m_isStream, compInfoOut);
    this->AddCompParInfo("isLoop", "bool", this->m_isLoop, compInfoOut);
    this->AddCompParInfo("initialVolume", "float", this->m_initialVolume, compInfoOut);
    this->AddCompParInfo("minDistance", "float", this->m_minDistance, compInfoOut);
    this->AddCompParInfo("maxDistance", "float", this->m_maxDistance, compInfoOut);
}

void AudioComponent::SetParameter(sParameterInfo& parameterIn)
{
    using namespace myutils;

    if (parameterIn.parameterName == "name") {
        this->m_name = parameterIn.parameterStrValue;
    }
    else if (parameterIn.parameterName == "idChannelGroup") {
        this->m_idChannelGroup = parameterIn.parameterIntValue;
    }
    else if (parameterIn.parameterName == "isStream") {
        this->m_isStream = parameterIn.parameterBoolValue;
    }
    else if (parameterIn.parameterName == "isLoop") {
        this->m_isLoop = parameterIn.parameterBoolValue;
    }
    else if (parameterIn.parameterName == "initialVolume") {
        this->m_initialVolume = parameterIn.parameterFloatValue;
    }
    else if (parameterIn.parameterName == "minDistance") {
        this->m_minDistance = parameterIn.parameterFloatValue;
    }
    else if (parameterIn.parameterName == "maxDistance") {
        this->m_maxDistance = parameterIn.parameterFloatValue;
    }

    return;
}

std::string AudioComponent::GetFileName()
{
    return this->m_name;
}

FMOD::Sound* AudioComponent::GetSound()
{
    return this->m_pSound;
}

bool AudioComponent::IsStream()
{
    if (this->m_isStream)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool AudioComponent::IsLoop()
{
    if (this->m_isLoop)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int AudioComponent::GetChannelGroup()
{
    return this->m_idChannelGroup;
}

float AudioComponent::GetInitialVolume()
{
    return this->m_initialVolume;
}

float AudioComponent::GetMinDistance()
{
    return this->m_minDistance;
}

float AudioComponent::GetMaxDistance()
{
    return this->m_maxDistance;
}

int AudioComponent::GetChannelId()
{
    return this->m_currentChannelId;
}

void AudioComponent::SetChannelId(int channelId)
{
    this->m_currentChannelId = channelId;
}

void AudioComponent::SetSound(FMOD::Sound* pSound)
{
    this->m_pSound = pSound;
}
