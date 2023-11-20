#include "events/MouseEvent.h"

MouseEvent::MouseEvent()
{
    m_sMouseInfo = sMouseInfo();
}

MouseEvent::~MouseEvent()
{
}

void MouseEvent::UpdateButton(int button, int action, int mods)
{
	m_sMouseInfo.pressedButton = button;
	m_sMouseInfo.action = action;
	m_sMouseInfo.mods = mods;
	this->Event::Notify("mousebuttonevent");
}

void MouseEvent::UpdatePosition(double xpos, double ypos)
{
	m_sMouseInfo.xpos = (float)xpos;
	m_sMouseInfo.ypos = (float)ypos;
	this->Event::Notify("mouseposevent");
}

sMouseInfo MouseEvent::GetInfo()
{
    return m_sMouseInfo;
}
