#pragma once
#include <entt/entity/registry.hpp>

#include "Engine/Engine.h"
#include <Events/CollisionEvent.h>

class MovementSystem 
{
public:
	MovementSystem() = default;


	void Update(entt::registry& registry, double delta_time);
	void SubscribeToEvents(std::unique_ptr<entt::dispatcher>& dispatcher)
	{
		dispatcher->sink<CollisionEvent>().connect<&MovementSystem::OnCollision>(this);
	}
	void OnCollision(CollisionEvent& event);
	void OnObstacleHitsEnemy(entt::entity& obstacle, entt::entity& enemy, entt::registry* registry);
};
