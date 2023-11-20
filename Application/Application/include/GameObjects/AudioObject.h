#pragma once

#include "DefaultObject.h"
#include "components/Audio.h"

class AudioObject : public DefaultObject
{
protected:
	AudioComponent* m_pAudio;
	bool m_isPlaying;			// Current playing status of the audio

	// Script custom
	//----------------
	bool m_startPlaying;		// If the audio should play on the start of the scene or not
	bool m_destroyAfter;
	bool m_isPausable;
	float m_pitch;

	bool m_setReverb;
	float m_reverbDecay;
	float m_reverbDensity;
	float m_reverbDiffusion;

	bool m_setDistortion;
	float m_distortionLevel;

	bool m_setHighPass;
	bool m_setLowPass;

public:
	AudioObject() {};
	virtual ~AudioObject() {};

	virtual glm::vec3 GetVelocity();

	virtual bool Load(EntityID entityID, SceneView* pScene);

	virtual void OnStart(iEvent* pEvent);

	virtual void Update(double deltaTime);

	virtual void OnExit(iEvent* pEvent);

	virtual void Interact(std::string tagThatInteracted, glm::vec3 position);
};
