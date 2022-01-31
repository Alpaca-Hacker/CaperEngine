#pragma once
#include <SDL.h>
#include <entt/entt.hpp>
#include "Event.h"

class KeypressEvent : public Event
{
public:
	SDL_Keycode symbol_;
	entt::registry& registry_;

	KeypressEvent(int symbol, entt::registry& registry) : symbol_(symbol), registry_(registry)
	{
		
	}
};


class KeyReleaseEvent : public Event
{
public:
	SDL_Keycode symbol_;
	entt::registry& registry_;

	KeyReleaseEvent(int symbol, entt::registry& registry) : symbol_(symbol), registry_(registry)
	{}
};