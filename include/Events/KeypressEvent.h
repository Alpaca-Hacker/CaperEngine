#pragma once
#include <SDL.h>

#include "Event.h"

class KeypressEvent : public Event
{
public:
	SDL_Keycode symbol;

	KeypressEvent(int symbol) : symbol(symbol) {}
};


class KeyReleaseEvent : public Event
{
public:
	SDL_Keycode symbol;

	KeyReleaseEvent(int symbol) : symbol(symbol) {}
};

