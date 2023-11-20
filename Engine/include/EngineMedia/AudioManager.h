#pragma once

#include "common/constants.h"

#include <string>
#include <glm/vec3.hpp>
#include <fmod/fmod.hpp>
#include <map>
#include <vector>

void GLMToFMOD(const glm::vec3& in, FMOD_VECTOR& out);
void FMODToGLM(const FMOD_VECTOR& in, glm::vec3& out);

void CheckError(FMOD_RESULT result, const char* file, int line);
#define FMODCheckError(result) CheckError(result, __FILE__, __LINE__)

class AudioManager
{
private:
	static const int m_REVERB_INDEX;
	static const int m_LOWPASS_INDEX;
	static const int m_HIGHPASS_INDEX;
	static const int m_DISTORTION_INDEX;
	static const int m_CHORUS_INDEX;

	bool m_isInitialized;
	int m_maxChannels;

	std::string m_basePath;

	std::map<std::string, FMOD::Sound*> m_mapAudio; // List of audios
	std::vector<FMOD::Channel*> m_vecChannel;	    // List of channels
	std::vector<FMOD::Geometry*> m_vecGeometry;	    // Occlusion geometries

	int m_idNextChannel;					        // The next channel Id that we can return

	FMOD::System* m_pSystem;

	// Audio DSP effects
	// Implemented
	FMOD::DSP* m_ReverbDSP;
	FMOD::DSP* m_HighPassDSP;
	FMOD::DSP* m_LowPassDSP;
	FMOD::DSP* m_DistortionDSP;
	FMOD::DSP* m_ChorusPassDSP;

	// Not implemented
	FMOD::DSP* m_DelayPassDSP;

	// Channel groups
	std::vector<FMOD::ChannelGroup*> m_vecChannelGroup;

	void m_CreateChannelGroups();

	inline FMOD::ChannelGroup* m_GetChannelGroup(int idChannelGroup)
	{
		return m_vecChannelGroup[idChannelGroup];
	}

	inline FMOD::ChannelGroup* m_GetMasterChannelGroup()
	{
		return m_vecChannelGroup[CHANNELGROUP_MASTER_INDEX];
	}
	inline FMOD::ChannelGroup* m_GetSFXChannelGroup()
	{
		return m_vecChannelGroup[CHANNELGROUP_SFX_INDEX];
	}
	inline FMOD::ChannelGroup* m_GetMusicChannelGroup()
	{
		return m_vecChannelGroup[CHANNELGROUP_MUSIC_INDEX];
	}
public:
	AudioManager();
	~AudioManager();

	// System calls
	bool Initialize(int maxChannels);
	void Update();
	void Destroy();

	// Clear and release resources
	void ClearAudios();
	void ClearChannels();
	void ClearOcclusions();

	void SetBasePath(std::string basePath);

	// Create the channel with FMOD
	int LoadChannel(FMOD::Channel* pChannel);

	// Loading Audio 
	bool LoadAudio(std::string file, bool isStream, bool isLoop, int idChannelGroup, FMOD::Sound** pFMODAudio);

	// Play audio in the next available channel
	int PlayAudio(std::string audioName, int idChannelGroup,
				  glm::vec3 soundPos, glm::vec3 soundVel,
				  float minDistance, float maxDistance);

	// Modifying Channel Values
	void SetChannel3DAttributes(int id, glm::vec3 soundPos, glm::vec3 soundVel);
	bool IsValidChannel(int id);
	bool StopChannel(int id);
	bool SetPaused(int id, bool value);
	bool SetChannelVolume(int id, float value);
	bool SetChannelPitch(int id, float value);
	bool SetChannelPan(int id, float value);

	// Retrieving information
	bool IsChannelPlaying(int id);
	bool IsChannelPaused(int id);
	bool IsAudioLoaded(std::string audioName);
	void GetAudioLength(std::string audioName, unsigned int& value);
	void GetPlaybackPosition(int id, unsigned int& value);
	void GetChannelPitch(int id, float& value);
	void GetChannelVolume(int id, float& value);

	// Channel groups
	void SetChannelGroupVolume(int id, float value);
	void SetChannelGroupPitch(int id, float value);
	void SetChannelGroupPan(int id, float value);

	// Set 3D audio attributes
	void SetListenerAttributes(const glm::vec3& position, const glm::vec3& velocity, 
							   const glm::vec3& forward, const glm::vec3& up);

	// Audio occlusion
	FMOD::Geometry* AddPolygon(float direct, float reverb, bool doublesided,
							   const std::vector<glm::vec3>& vertices, const glm::vec3& position);

	// Audio DSP effects

	// Sets the amount by which doppler is scaled
	void SetDopplerLevel(int channelId, float level);

	// REVERB DSP EFFECT
	void AddReverbFilterOnChannel(int channelId);

	// LOW PASS FILTER DSP EFFECT
	void AddLowPassFilterOnChannel(int channelId);

	// HIGH PASS FILTER DSP EFFECT
	void AddHighPassFilterOnChannel(int channelId);

	// DISTORTION PASS FILTER DSP EFFECT
	void AddDistortionFilterOnChannel(int channelId);

	// CHROSU PASS FILTER DSP EFFECT
	void AddChorusPassOnChannel(int channelId);

	// MODIFYING REVERB DSP EFFECT
	void SetReverbValuesOnChannel(int channelId, float decay, float density, float diffusion);

	// MODIFYING LOW PASS FILTER DSP EFFECT
	void SetLowPassFilterValuesOnChannel(int channelId, float cutoff);

	// MODIFYING HIGH PASS FILTER DSP EFFECT
	void SetHighPassFilterValuesOnChannel(int channelId, float cutoff);

	// MODIFYING DISTORTION PASS FILTER DSP EFFECT
	void SetDistortionLevelFilterValuesOnChannel(int channelId, float level);

	// MODIFYING CHROUS PASS FILTER DSP EFFECT
	void SetChorusPassValuesOnChannel(int channelId, float mix, float rate, float depth);
};