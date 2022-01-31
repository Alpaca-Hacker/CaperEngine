#pragma once
#include <SDL.h>
#include <entt/entt.hpp>

class CameraMovementSystem 
{
public:

	void Update(entt::registry& registry, SDL_Rect& camera);
};
