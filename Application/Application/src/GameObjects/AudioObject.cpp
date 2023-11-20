#include "GameObjects/AudioObject.h"
#include "EngineMedia/MediaPlayer.h"

glm::vec3 AudioObject::GetVelocity()
{
	// Motionless audio for default
	return glm::vec3(0.0f);
}

bool AudioObject::Load(EntityID entityID, SceneView* pScene)
{
	this->DefaultObject::Load(entityID, pScene);

	this->m_pAudio = pScene->GetComponent<AudioComponent>(entityID, "audio");

	return true;
}

void AudioObject::OnStart(iEvent* pEvent)
{
	this->DefaultObject::OnStart(pEvent);

	// Script custom
	//----------------
	this->m_destroyAfter = std::stoi(this->m_pScript->GetVariable("destroyAfter"));
	this->m_startPlaying = std::stoi(this->m_pScript->GetVariable("startPlaying"));
	this->m_isPausable = std::stoi(this->m_pScript->GetVariable("isPausable"));
	this->m_pitch = std::stof(this->m_pScript->GetVariable("pitch"));
	
	// Reverberation
	this->m_setReverb = std::stoi(this->m_pScript->GetVariable("setReverb"));
	this->m_reverbDecay = std::stof(this->m_pScript->GetVariable("reverbDecay"));
	this->m_reverbDensity = std::stof(this->m_pScript->GetVariable("reverbDensity"));
	this->m_reverbDiffusion = std::stof(this->m_pScript->GetVariable("reverbDiffusion"));

	// Distortion
	this->m_setDistortion = std::stoi(this->m_pScript->GetVariable("setDistortion"));
	this->m_distortionLevel = std::stof(this->m_pScript->GetVariable("distortionLevel"));

	// HighPass
	this->m_setHighPass = std::stoi(this->m_pScript->GetVariable("setHighPass"));

	// Lowpass
	this->m_setLowPass = std::stoi(this->m_pScript->GetVariable("setLowPass"));

	if (this->m_startPlaying)
	{
		this->Interact(this->GetTagName(), this->GetPosition());
	}
}

void AudioObject::Update(double deltaTime)
{
	if (this->m_pAudio->GetChannelId() == -1)
	{
		return;
	}

	MediaPlayer* pMediaPlayer = MediaPlayer::Get();

	// Check if the music already finished to reset channel
	if (pMediaPlayer->IsFinished(this->GetEntityID()))
	{
		this->m_isPlaying = false;
		this->m_pAudio->SetChannelId(-1);
	}
}

void AudioObject::OnExit(iEvent* pEvent)
{
	this->DefaultObject::OnExit(pEvent);

	MediaPlayer::Get()->StopAudio(this->GetEntityID());
}

void AudioObject::Interact(std::string tagThatInteracted, glm::vec3 position)
{
	MediaPlayer* pMediaPlayer = MediaPlayer::Get();
	EntityID entityId = this->GetEntityID();

	if(this->m_isPlaying && !this->m_isPausable)
	{
		return;
	}

	this->m_isPlaying = !this->m_isPlaying;
	if (this->m_pAudio->GetChannelId() > -1)
	{
		// Audio already playing, so just pause channel
		pMediaPlayer->PauseAudio(entityId, this->m_isPlaying);
		return;
	}
	
	// First time playing
	pMediaPlayer->PlayAudio(entityId, this->GetVelocity());
	pMediaPlayer->SetPitch(entityId, this->m_pitch);

	if (this->m_setReverb)
	{
		pMediaPlayer->SetReverberation(entityId, this->m_reverbDecay,
			this->m_reverbDensity, this->m_reverbDiffusion);
	}

	if (this->m_setDistortion)
	{
		pMediaPlayer->SetDistortion(entityId, this->m_distortionLevel);
	}

	if (this->m_setHighPass)
	{
		pMediaPlayer->SetHighpass(entityId);
	}

	if (this->m_setLowPass)
	{
		pMediaPlayer->SetLowpass(entityId);
	}

	if (this->m_destroyAfter)
	{
		this->GetMediator()->DestroyGameObj(this);
	}
}
