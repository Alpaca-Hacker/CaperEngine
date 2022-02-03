#pragma once
#include <SDL.h>

#include <entt/entt.hpp>


struct KeypressEvent 
{
	SDL_Keycode symbol;
	entt::registry *registry;

	KeypressEvent(int symbol = 0, entt::registry *registry = nullptr) : symbol(symbol), registry(registry)
	{}
};


struct KeyReleaseEvent 
{
	SDL_Keycode symbol;
	entt::registry* registry;

	KeyReleaseEvent(int symbol = 0,  entt::registry * registry = nullptr) : symbol(symbol), registry(registry)
	{}
};