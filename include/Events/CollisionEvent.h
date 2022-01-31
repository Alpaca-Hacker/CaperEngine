#pragma once
#include <entt/entt.hpp>

#include "Events/Event.h"

class CollisionEvent : public Event
{
public:
	entt::entity a;
	entt::entity b;

	CollisionEvent(entt::entity a, entt::entity b) : a(a), b(b) {}
};
