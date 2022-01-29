#pragma once
#include <SDL.h>

struct ProjectileComponent
{
	bool is_friendly;
	int damage;
	int duration;
	int start_time;

	ProjectileComponent(bool is_friendly = false, int damage = 0, int duration = 0)
	: is_friendly(is_friendly), damage(damage), duration(duration), start_time(SDL_GetTicks64())
	{  }
};
