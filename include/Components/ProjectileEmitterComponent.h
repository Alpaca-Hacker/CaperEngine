#pragma once
#include <SDL.h>


struct ProjectileEmitterComponent
{
	int projectile_speed;
	int repeat_frequency;
	int projectile_duration;
	int hit_percent_damage;
	bool is_friendly;
	int last_emission_time;

	ProjectileEmitterComponent(int projectile_speed = 0, int repeat_freq = 0, int projectile_duration = 1000, 
		int hit_damage = 0, bool is_friendly = false)
	:	projectile_speed(projectile_speed),
		repeat_frequency(repeat_freq),
		projectile_duration(projectile_duration),
		hit_percent_damage(hit_damage),
		is_friendly(is_friendly),
		last_emission_time(SDL_GetTicks64())
	{  }

};
