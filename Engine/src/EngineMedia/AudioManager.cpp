#include "EngineMedia/AudioManager.h"
#include "common/utils.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>

const int AudioManager::m_REVERB_INDEX = 0;
const int AudioManager::m_LOWPASS_INDEX = 1;
const int AudioManager::m_HIGHPASS_INDEX = 2;
const int AudioManager::m_DISTORTION_INDEX = 3;
const int AudioManager::m_CHORUS_INDEX = 4;

void GLMToFMOD(const glm::vec3& in, FMOD_VECTOR& out)
{
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
}
void FMODToGLM(const FMOD_VECTOR& in, glm::vec3& out)
{
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
}

void CheckError(FMOD_RESULT result, const char* file, int line)
{
	if (result != FMOD_OK)
	{
		printf("FMOD Error [%d]: '%s' at %d\n", static_cast<int>(result), file, line);
	}
}

AudioManager::AudioManager()
{
	this->m_isInitialized = false;

	this->m_mapAudio = {};
	this->m_vecChannel = {};

	this->m_idNextChannel = 0;

	this->m_pSystem = nullptr;
}

AudioManager::~AudioManager()
{
}

void AudioManager::m_CreateChannelGroups()
{
	FMOD_RESULT result;

	FMOD::ChannelGroup* channelGroup;
	result = this->m_pSystem->createChannelGroup("Master", &channelGroup);
	FMODCheckError(result);
	this->m_vecChannelGroup.push_back(channelGroup);

	result = this->m_pSystem->createChannelGroup("SFX", &channelGroup);
	FMODCheckError(result);
	this->m_vecChannelGroup.push_back(channelGroup);

	result = this->m_pSystem->createChannelGroup("Music", &channelGroup);
	FMODCheckError(result);
	this->m_vecChannelGroup.push_back(channelGroup);

	// Add our SFX and Music Channel Groups to our Master ChannelGroup
	result = m_GetMasterChannelGroup()->addGroup(m_GetSFXChannelGroup());
	FMODCheckError(result);

	result = m_GetMasterChannelGroup()->addGroup(m_GetMusicChannelGroup());
	FMODCheckError(result);
}

bool AudioManager::Initialize(int maxChannels)
{
	// Initializes FMOD system and creates #channels in vector
	if (this->m_isInitialized)
	{
		return true;
	}

	this->m_maxChannels = maxChannels;

	FMOD_RESULT result;
	result = FMOD::System_Create(&this->m_pSystem);
	if (result != FMOD_OK)
	{
		FMODCheckError(result);
		return false;
	}

	result = this->m_pSystem->init(this->m_maxChannels, FMOD_INIT_NORMAL | FMOD_INIT_PROFILE_ENABLE, nullptr);
	if (result != FMOD_OK)
	{
		FMODCheckError(result);
		// Cleanup
		result = this->m_pSystem->close();
		FMODCheckError(result);
		return false;
	}

	printf("AudioManager::Initialize(): Successful!\n");

	this->m_CreateChannelGroups();

	result = this->m_pSystem->set3DSettings(.5f, .5f, 0.1f);
	FMODCheckError(result);
	result = this->m_pSystem->setGeometrySettings(100.0f);
	FMODCheckError(result);

	this->m_isInitialized = true;

	return true;
}

void AudioManager::Destroy()
{
	// Releases all audio and FMOD system from memory
	if (!this->m_isInitialized)
		return;

	FMOD_RESULT result;

	this->ClearAudios();
	this->ClearChannels();
	this->ClearOcclusions();

	result = this->m_pSystem->close();
	FMODCheckError(result);

	result = this->m_pSystem->release();
	FMODCheckError(result);

	this->m_isInitialized = false;
}

void AudioManager::ClearAudios()
{
	// Properly releases the audio using FMOD
	FMOD_RESULT result;
	for (std::pair<std::string, FMOD::Sound*> pair : this->m_mapAudio)
	{
		result = pair.second->release();
		FMODCheckError(result);
	}

	this->m_mapAudio.clear();
}

void AudioManager::ClearChannels()
{
	this->m_vecChannel.clear();
}

void AudioManager::ClearOcclusions()
{
	this->m_vecGeometry.clear();
}

void AudioManager::SetBasePath(std::string basePath)
{
	this->m_basePath = basePath;
}

int AudioManager::LoadChannel(FMOD::Channel* pChannel)
{
	if (this->m_vecChannel.size() >= this->m_maxChannels)
	{
		CheckEngineError("Maximum channels reached");
		return -1;
	}

	this->m_vecChannel.push_back(pChannel);

	return (int)this->m_vecChannel.size();
}

/**
  * LoadAudio - Checks if the provided file is already loaded in our
  * cache, if it is not in our cache we can load it.
  *
  * @param file: Audio file name to load
  * @param loadType: Which type will be used to load this file into memory (1 - sample, 2 - stream)
  *
  * @return: The name of the audio file used as key
  */
bool AudioManager::LoadAudio(std::string file, bool isStream, bool isLoop, int idChannelGroup, FMOD::Sound** pSound)
{
	// If our AudioManager is not initialized, we shouldn't do anything
	if (!this->m_isInitialized)
	{
		CheckEngineError("Audio manager not initialized!");
		return false;
	}

	std::string fullPath = this->m_basePath + file;

	// Search our cache if we have already loaded this file
	// If we have already loaded the file, return
	if (this->IsAudioLoaded(file))
	{
		CheckEngineError("Audio already loaded!");
		return true;
	}

	// TODO: Option to load audios that are not 3D

	FMOD_RESULT result;
	FMOD_MODE mode = FMOD_3D;
	if (isLoop)
	{
		mode |= FMOD_LOOP_NORMAL;
	}

	if (isStream)
	{
		// This will only load the audio resource data as needed
		// to play the audio.
		result = this->m_pSystem->createStream(fullPath.c_str(), mode, 0, pSound);
	}
	else
	{
		// This call loads our audio entirely into memory
		result = this->m_pSystem->createSound(fullPath.c_str(), mode, 0, pSound);
	}

	if (result != FMOD_OK)
	{
		std::string errorMsg = "Error loading file: " + fullPath;
		CheckEngineError(errorMsg.c_str());
		return false;
	}

	this->m_mapAudio.insert(std::pair<std::string, FMOD::Sound*>(file, *pSound));

	return true;
}

int AudioManager::PlayAudio(std::string audioName, int idChannelGroup, 
							glm::vec3 soundPos, glm::vec3 soundVel,
							float minDistance, float maxDistance)
{
	// If our AudioManager is not initialized, we shouldn't do anything
	if (!this->m_isInitialized)
	{
		printf("Not initialized!\n");
		return this->m_idNextChannel;	// Still want to return a valid channel id
	}

	// Check our cache to see if the audio exists.
	std::map<std::string, FMOD::Sound*>::iterator it = this->m_mapAudio.find(audioName);
	if (it == this->m_mapAudio.end())
	{
		printf("Audio not found!\n");
		return this->m_idNextChannel;	// Still want to return a valid channel id
	}

	// Get the current channel id, and calculate the next one
	int idChannel = this->m_idNextChannel;
	this->m_idNextChannel = (this->m_idNextChannel + 1) % this->m_maxChannels;

	// FMOD::ChannelGroup* pChannelGroup = this->m_vecChannelGroup[idChannelGroup];
	FMOD::ChannelGroup* pChannelGroup = nullptr;

	// Attemp to play our sound.
	FMOD_RESULT result = this->m_pSystem->playSound(it->second,
													pChannelGroup,
													false,
													&this->m_vecChannel[idChannel]);
	if (result != FMOD_OK)
	{
		FMODCheckError(result);
		return this->m_idNextChannel;
	}

	this->SetChannel3DAttributes(idChannel, soundPos, soundVel);
	result = this->m_vecChannel[idChannel]->set3DMinMaxDistance(minDistance, maxDistance);
	FMODCheckError(result);

	result = it->second->set3DMinMaxDistance(minDistance, maxDistance);
	FMODCheckError(result);

	// Return the channel id used so we can modify it if needed.
	return idChannel;
}

void AudioManager::SetChannel3DAttributes(int id, glm::vec3 soundPos, glm::vec3 soundVel)
{
	// Convert from GLM vector to FMOD vector
	FMOD_VECTOR FMODsoundPos;
	FMOD_VECTOR FMODsoundVel;
	GLMToFMOD(soundPos, FMODsoundPos);
	GLMToFMOD(soundVel, FMODsoundVel);

	// Update the channel`s 3d attributes and distances
	FMOD_RESULT result;
	result = this->m_vecChannel[id]->set3DAttributes(&FMODsoundPos, &FMODsoundVel);
	FMODCheckError(result);

	return;
}

bool AudioManager::IsValidChannel(int id)
{
	// If our AudioManager is not initialized, we shouldn't do anything
	if (!this->m_isInitialized)
	{
		CheckEngineError("Not initialized!");
		return false;
	}

	if (id < 0 || id >= this->m_vecChannel.size())
	{
		std::string errorMsg = "Channel id not in list: " + std::to_string(id);
		CheckEngineError(errorMsg.c_str());
		return false;
	}

	return true;
}

bool AudioManager::StopChannel(int id)
{
	if (!this->IsValidChannel(id))
	{
		return false;
	}

	FMOD_RESULT result = this->m_vecChannel[id]->stop();
	if (result == FMOD_OK)
	{
		return true;
	}
	else
	{
		FMODCheckError(result);
		return false;
	}
}

bool AudioManager::SetPaused(int id, bool value)
{
	if (!this->IsValidChannel(id))
	{
		return false;
	}

	FMOD_RESULT result = this->m_vecChannel[id]->setPaused(value);
	if (result == FMOD_OK)
	{
		return true;
	}
	else
	{
		FMODCheckError(result);
		return false;
	}
}

void AudioManager::Update()
{
	// If our AudioManager is not initialized, we shouldn't do anything
	if (!this->m_isInitialized)
	{
		return;
	}

	// Call system update, this needs to occur once in a while. It is 
	// not mandatory for each frame.
	FMOD_RESULT result = this->m_pSystem->update();

	if (result != FMOD_OK)
	{
		FMODCheckError(result);
		Destroy();
	}
}

bool AudioManager::SetChannelVolume(int id, float value)
{
	// This call sets the volume on the channel. We can't modify 
	// the value, we can only set it.
	if (!this->IsValidChannel(id))
	{
		return false;
	}

	FMOD_RESULT result = this->m_vecChannel[id]->setVolume(value);
	if (result == FMOD_OK)
	{
		return true;
	}
	else
	{
		FMODCheckError(result);
		return false;
	}
}

bool AudioManager::SetChannelPitch(int id, float value)
{
	// This call sets the pitch value on the channel. We can't modify 
	// the value, we can only set it.
	if (!this->IsValidChannel(id))
	{
		return false;
	}

	FMOD_RESULT result = this->m_vecChannel[id]->setPitch(value);
	if (result == FMOD_OK)
	{
		return true;
	}
	else
	{
		FMODCheckError(result);
		return false;
	}
}

bool AudioManager::SetChannelPan(int id, float value)
{
	// This call sets the pan value on the channel. We can't modify 
	// the value, we can only set it.
	if (!this->IsValidChannel(id))
	{
		return false;
	}

	FMOD_RESULT result = this->m_vecChannel[id]->setPan(value);
	if (result == FMOD_OK)
	{
		return true;
	}
	else
	{
		FMODCheckError(result);
		return false;
	}
}

bool AudioManager::IsChannelPlaying(int id)
{
	if (!this->IsValidChannel(id))
	{
		return false;
	}

	// Check if the channel id is currently playing or not.
	// If it is not playing we can do a cleanup
	bool isPlaying = false;
	FMOD_RESULT result = this->m_vecChannel[id]->isPlaying(&isPlaying);
	return isPlaying;
}

bool AudioManager::IsChannelPaused(int id)
{
	if (!this->IsValidChannel(id))
	{
		return false;
	}

	// Check if the channel id is currently paused.
	bool isPaused = false;
	FMOD_RESULT result = this->m_vecChannel[id]->getPaused(&isPaused);
	return isPaused;
}

void AudioManager::GetPlaybackPosition(int id, unsigned int& value)
{
	if (!this->IsValidChannel(id))
	{
		value = 0;
		return;
	}

	// Use this call to get the playback position of a channel.
	FMOD_RESULT result = this->m_vecChannel[id]->getPosition(&value, FMOD_TIMEUNIT_MS);
	FMODCheckError(result);
}

bool AudioManager::IsAudioLoaded(std::string audioName)
{
	// Check our cache to see if the audio exists.
	std::map<std::string, FMOD::Sound*>::iterator it = this->m_mapAudio.find(audioName);
	if (it == this->m_mapAudio.end())
	{
		// Audio not found
		return false;
	}

	return true;
}

void AudioManager::GetAudioLength(std::string audioName, unsigned int& value)
{
	std::map<std::string, FMOD::Sound*>::iterator it = this->m_mapAudio.find(audioName);
	if (it == this->m_mapAudio.end())
	{
		// Audio not found
		return;
	}

	FMOD_RESULT result = it->second->getLength(&value, FMOD_TIMEUNIT_MS);
	FMODCheckError(result);
}

void AudioManager::GetChannelPitch(int id, float& value)
{
	// Use this call to get the pitch value of a channel.
	FMOD_RESULT result = this->m_vecChannel[id]->getPitch(&value);
	FMODCheckError(result);
}

void AudioManager::GetChannelVolume(int id, float& value)
{
	// Use this call to get the volume value of a channel.
	FMOD_RESULT result = this->m_vecChannel[id]->getVolume(&value);
	FMODCheckError(result);
}

void AudioManager::SetChannelGroupVolume(int id, float value)
{
	printf("Set ChannelGroup Volume %d to %.2f\n", id, value);
	FMOD_RESULT result = this->m_vecChannelGroup[id]->setVolume(value);
	FMODCheckError(result);
}

void AudioManager::SetChannelGroupPitch(int id, float value)
{
	FMOD_RESULT result = this->m_vecChannelGroup[id]->setPitch(value);
	FMODCheckError(result);
}

void AudioManager::SetChannelGroupPan(int id, float value)
{
	FMOD_RESULT result = this->m_vecChannelGroup[id]->setPan(value);
	FMODCheckError(result);
}

void AudioManager::SetListenerAttributes(const glm::vec3& position, const glm::vec3& velocity, 
										 const glm::vec3& forward, const glm::vec3& up)
{
	FMOD_VECTOR fmodPosition;
	FMOD_VECTOR fmodVelocity;
	FMOD_VECTOR fmodForward;
	FMOD_VECTOR fmodUp;

	GLMToFMOD(position, fmodPosition);
	GLMToFMOD(velocity, fmodVelocity);
	GLMToFMOD(forward, fmodForward);
	GLMToFMOD(up, fmodUp);

	FMOD_RESULT result = this->m_pSystem->set3DListenerAttributes(
		0, &fmodPosition, &fmodVelocity, &fmodForward, &fmodUp
	);

	FMODCheckError(result);
}

FMOD::Geometry* AudioManager::AddPolygon(float direct, float reverb, bool doublesided, 
										 const std::vector<glm::vec3>& vertices, const glm::vec3& position)
{
	FMOD_RESULT result;
	int index;

	// Add the polygon
	int numVertices = (int)vertices.size();
	FMOD_VECTOR* fmodVertices = (FMOD_VECTOR*)malloc(sizeof(FMOD_VECTOR) * numVertices);
	for (int i = 0; i < numVertices; i++) {
		GLMToFMOD(vertices[i], fmodVertices[i]);
	}

	FMOD::Geometry* pGeometry;

	result = this->m_pSystem->createGeometry(2000, 8000, &pGeometry);
	FMODCheckError(result);

	result = pGeometry->addPolygon(direct, reverb, doublesided, numVertices, fmodVertices, &index);
	FMODCheckError(result);

	// Set the position
	FMOD_VECTOR fmodPosition;
	GLMToFMOD(position, fmodPosition);
	result = pGeometry->setPosition(&fmodPosition);
	FMODCheckError(result);

	glm::vec3 scale(1.f);
	FMOD_VECTOR fmodScale;
	GLMToFMOD(scale, fmodScale);
	result = pGeometry->setScale(&fmodScale);
	FMODCheckError(result);

	FMOD_VECTOR v;
	glm::vec3 g;
	for (int i = 0; i < numVertices; i++)
	{
		pGeometry->getPolygonVertex(index, i, &v);
		FMODToGLM(v, g);
	}

	pGeometry->setActive(true);

	this->m_vecGeometry.push_back(pGeometry);

	return pGeometry;
}

void AudioManager::SetDopplerLevel(int channelId, float level)
{
	FMOD_RESULT result = this->m_vecChannel[channelId]->set3DDopplerLevel(level);
	FMODCheckError(result);
}

void AudioManager::AddReverbFilterOnChannel(int channelId)
{
	FMOD_RESULT result = this->m_pSystem->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &m_ReverbDSP);
	FMODCheckError(result);

	this->m_vecChannel[channelId]->addDSP(this->m_REVERB_INDEX, m_ReverbDSP);
}

void AudioManager::AddLowPassFilterOnChannel(int channelId)
{
	FMOD_RESULT result = this->m_pSystem->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &m_LowPassDSP);
	FMODCheckError(result);

	result = m_LowPassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, 5000);
	FMODCheckError(result);

	this->m_vecChannel[channelId]->addDSP(this->m_LOWPASS_INDEX, m_LowPassDSP);
}

void AudioManager::AddHighPassFilterOnChannel(int channelId)
{
	FMOD_RESULT result = this->m_pSystem->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &m_HighPassDSP);
	FMODCheckError(result);

	m_HighPassDSP->setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, 500);
	FMODCheckError(result);

	this->m_vecChannel[channelId]->addDSP(this->m_HIGHPASS_INDEX, m_HighPassDSP);
}

void AudioManager::AddDistortionFilterOnChannel(int channelId)
{
	FMOD_RESULT result = this->m_pSystem->createDSPByType(FMOD_DSP_TYPE_DISTORTION, &m_DistortionDSP);
	FMODCheckError(result);

	m_DistortionDSP->setParameterFloat(FMOD_DSP_DISTORTION_LEVEL, 0.7f);
	FMODCheckError(result);

	this->m_vecChannel[channelId]->addDSP(this->m_DISTORTION_INDEX, m_DistortionDSP);
}

void AudioManager::AddChorusPassOnChannel(int channelId)
{
	FMOD_RESULT result = this->m_pSystem->createDSPByType(FMOD_DSP_TYPE_CHORUS, &m_ChorusPassDSP);
	FMODCheckError(result);

	m_ChorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_MIX, 50.f);
	m_ChorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_RATE, 0.8f);
	m_ChorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_DEPTH, 3.f);
	FMODCheckError(result);

	this->m_vecChannel[channelId]->addDSP(this->m_CHORUS_INDEX, m_ChorusPassDSP);
}

void AudioManager::SetReverbValuesOnChannel(int channelId, float decay, float density, float diffusion)
{
	FMOD_RESULT result;

	// Concert Hall, settings, decay 3900ms, density, diffusion set to 100

	result = m_ReverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DECAYTIME, decay);
	result = m_ReverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DENSITY, density);
	result = m_ReverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DIFFUSION, diffusion);
	FMODCheckError(result);
}

void AudioManager::SetLowPassFilterValuesOnChannel(int channelId, float cutoff)
{
	FMOD_RESULT result = m_LowPassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, cutoff);
	FMODCheckError(result);
}

void AudioManager::SetHighPassFilterValuesOnChannel(int channelId, float cutoff)
{
	FMOD_RESULT result = m_HighPassDSP->setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, cutoff);
	FMODCheckError(result);
}

void AudioManager::SetDistortionLevelFilterValuesOnChannel(int channelId, float level)
{
	FMOD_RESULT result = m_DistortionDSP->setParameterFloat(FMOD_DSP_DISTORTION_LEVEL, level);
	FMODCheckError(result);
}

void AudioManager::SetChorusPassValuesOnChannel(int channelId, float mix, float rate, float depth)
{
	FMOD_RESULT result;
	result = m_ChorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_MIX, mix);
	result = m_ChorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_RATE, rate);
	result = m_ChorusPassDSP->setParameterFloat(FMOD_DSP_CHORUS_DEPTH, depth);
	FMODCheckError(result);
}