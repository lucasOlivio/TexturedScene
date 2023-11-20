#pragma once

#include "Event.h"

struct sMouseInfo
{
	int pressedButton;
	int action;
	int mods;
	float xpos;
	float ypos;
};

// Mouse callback events triggered by glfw on window
class MouseEvent : public Event
{
private:
	sMouseInfo m_sMouseInfo;
public:
	MouseEvent();
	virtual ~MouseEvent();

	void UpdateButton(int button, int action, int mods);
	void UpdatePosition(double xpos, double ypos);

	sMouseInfo GetInfo();
};