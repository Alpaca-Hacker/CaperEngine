#pragma once
#include <SDL.h>

#include "Event.h"

class KeypressEvent : public Event
{
public:
	SDL_Keycode symbol;

	KeypressEvent(int symbol) : symbol(symbol) {}
};
