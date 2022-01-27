#pragma once
#include "glm/vec2.hpp"

struct BoxColliderComponent
{
	int width;
	int height;
	glm::vec2 offset;
	bool is_hit;

	BoxColliderComponent(int width = 0, int height = 0, glm::vec2 offset = glm::vec2(0))
		: width(width), height(height), offset(offset), is_hit(false)
	{  }
};
