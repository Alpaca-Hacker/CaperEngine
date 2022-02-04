#pragma once
#include <SDL.h>
#include <string>

#include "glm/vec2.hpp"

struct TextComponent
{
	glm::vec2 position;
	std::string text;
	std::string asset_id;
	SDL_Color colour;
	bool is_fixed;

	TextComponent(glm::vec2 position = glm::vec2(0), std::string text = "", std::string asset_id = "", const SDL_Color colour = {0,0,0, 0}, bool is_fixed = true)
		: position(position), text(text), asset_id(asset_id), colour(colour), is_fixed(is_fixed)

	{}
};
