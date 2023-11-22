#pragma once

#include "events/KeyEvent.h"
#include "events/MouseEvent.h"
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

// Hazel engine from "The Cherno" - src/Hazel/Core/Input.h
class Input : public iListener
{
public:
	Input(KeyEvent* pKeyevent, MouseEvent* pMouseEvent);

	virtual void Notify(std::string eventName, iEvent* pEvent);

	static bool IsKeyPressed(int key);

	static glm::vec2 MousePosition();

	static glm::vec2 MousePosRelative();

	static bool IsMouseButtonPressed(int button);

	static int GetKeyMods();
	static int GetMouseMods();

	static int GetKeyAction();
	static int GetMouseAction();

	static sKeyInfo GetKeyInfo();
	static sMouseInfo GetMouseInfo();

private:
	static sKeyInfo m_keyInfo;
	static sMouseInfo m_mouseInfo;
};