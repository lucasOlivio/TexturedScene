#pragma once

#include "Component.h"
#include <glm/vec3.hpp>
#include <fmod/fmod.hpp>

class ChannelComponent : public Component
{
private:
	bool m_isActive;
	std::string m_name;
	FMOD::Channel* m_pChannel;
	float m_volume;
	float m_pitch;
	float m_pan;
	bool m_isPaused;

	int m_channelId;

public:
	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);

	FMOD::Channel* GetChannel();

	void SetVolume(float value);
	void SetPitch(float value);
	void SetPan(float value);
	void SetPaused(bool value);
	void Stop();

	float GetPan();
};