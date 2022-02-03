#pragma once
#include <entt/entt.hpp>

struct CollisionEvent 
{
public:
	entt::entity a;
	entt::entity b;
	entt::registry* registry;

	CollisionEvent(entt::entity a, entt::entity b, entt::registry* registry = nullptr) : a(a), b(b), registry(registry) {}
};
