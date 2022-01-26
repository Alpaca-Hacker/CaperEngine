#pragma once

struct SpriteComponent
{
	std::string asset_id;
	int width;
	int height;
	SDL_Rect src_rect;

	SpriteComponent(std::string asset_id = "", int width = 1, int height = 1, int src_rect_x = 0, int src_rect_y = 0) : asset_id(std::move(asset_id)), width(width), height(height)
	{
		this->src_rect = { src_rect_x, src_rect_y, width, height };
	}
};
