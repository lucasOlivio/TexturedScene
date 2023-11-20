#include "events/GLFWWrapper.h"

KeyEvent* GLFWWrapper::m_pKeyEvent = nullptr;
MouseEvent* GLFWWrapper::m_pMouseEvent = nullptr;

void GLFWWrapper::SetKeyEvent(KeyEvent* pKeyEvent)
{
	m_pKeyEvent = pKeyEvent;
}

void GLFWWrapper::SetMouseEvent(MouseEvent* pMouseEvent)
{
	m_pMouseEvent = pMouseEvent;
}

void GLFWWrapper::MousePressCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (!m_pMouseEvent)
	{
		// Event not configured
		return;
	}
	m_pMouseEvent->UpdateButton(button, action, mods);
}

void GLFWWrapper::MousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (!m_pMouseEvent)
	{
		// Event not configured
		return;
	}
	m_pMouseEvent->UpdatePosition(xpos, ypos);
}

void GLFWWrapper::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (!m_pKeyEvent || !key)
	{
		// Event not configured
		return;
	}
	m_pKeyEvent->UpdateKey(key, scancode, action, mods);
}
