#pragma once

#include "KeyEvent.h"
#include "MouseEvent.h"
#include "common/opengl.h"

// Wrapper to send callbacks to events from GLFW
class GLFWWrapper
{
private:
	static KeyEvent* m_pKeyEvent;
	static MouseEvent* m_pMouseEvent;
public:
	// Key callbacks
	static void SetKeyEvent(KeyEvent* pKeyEvent);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	// Mouse callbacks
	static void SetMouseEvent(MouseEvent* pMouseEvent);
	static void MousePressCallback(GLFWwindow* window, int button, int action, int mods);
	static void MousePosCallback(GLFWwindow* window, double xpos, double ypos);
};