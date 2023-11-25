#pragma once

#include "scene/SceneView.h"
#include "AudioManager.h"
#include <glm/mat4x4.hpp>

class MediaPlayer
{
private:
	AudioManager* m_pAudioManager;
	SceneView* m_pSceneView;

	//Singleton
	MediaPlayer();

	static MediaPlayer* m_pInstance;
public:
	~MediaPlayer();

	static MediaPlayer* Get();

	// Where we really start/destroy everything that could go wrong
	bool Initialize(std::string baseAudiosPath, SceneView* pSceneView);
	bool LoadScene();
	bool LoadOcclusions();
	bool LoadChannels();
	bool LoadAudios();
	void Destroy();

	void Update(double deltaTime);

	void SetListenerAttributes(const glm::vec3& position, const glm::vec3& velocity,
								const glm::vec3& forward, const glm::vec3& up);

	void SetAudio3DAttributes(EntityID entityId, const glm::vec3& position, const glm::vec3& velocity);

	// Manage channel
	void PlayAudio(EntityID entityId, glm::vec3 soundVelocity);
	void StopAudio(EntityID entityId);
	void PauseAudio(EntityID entityId, bool value);

	bool SetPitch(EntityID entityId, float value);

	int GetChannelId(EntityID entityId);

	bool IsPlaying(EntityID entityId);
	// Check if the music is over
	bool IsFinished(EntityID entityId);

	// Channel effects
	void SetDopplerLevel(EntityID entityId, float level);
	void SetReverberation(EntityID entityId, float decay, float density, float diffusion);
	void SetDistortion(EntityID entityId, float level);
	void SetHighpass(EntityID entityId);
	void SetLowpass(EntityID entityId);
};