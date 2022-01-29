#pragma once

struct SpriteComponent
{
	std::string asset_id;
	int width;
	int height;
	int z_index;
	SDL_Rect src_rect;
	bool is_fixed;

	SpriteComponent(std::string asset_id = "", int width = 1, int height = 1, int  z_index = 0, bool is_fixed = false, int src_rect_x = 0, int src_rect_y = 0)
		: asset_id(std::move(asset_id)), width(width), height(height), z_index(z_index), is_fixed(is_fixed)
	{
		this->src_rect = { src_rect_x, src_rect_y, width, height };
	}
};
