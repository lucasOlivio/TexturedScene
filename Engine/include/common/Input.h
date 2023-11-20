#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

// Hazel engine from "The Cherno" - src/Hazel/Core/Input.h
class Input
{
public:
	static bool IsKeyPressed(int key);
	static glm::vec2 MousePosition();

	static glm::vec2 MousePosRelative();
};