#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define MAX_LINE_LENGTH 65536		// 16x1024
#define PI 3.14f

#define CHANNELGROUP_MASTER_INDEX	0
#define CHANNELGROUP_MUSIC_INDEX	1
#define CHANNELGROUP_SFX_INDEX		2

const glm::vec3 FORWARD_VECTOR = glm::vec3(0, 0, -1);

const int    FRAME_RATE     = 30;  // Frames per second
const double FRAME_DURATION = 1.0 / FRAME_RATE;

const glm::vec4 RED = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 GREEN = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
const glm::vec4 BLUE = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);