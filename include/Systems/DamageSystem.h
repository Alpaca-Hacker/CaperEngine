#pragma once
#include "Events/CollisionEvent.h"
#include "Events/EventBus.h"

class DamageSystem 
{
public:
	entt::registry& registry_;
	DamageSystem(entt::registry& registry) : registry_(registry) {  }

	void SubscribeToEvents(std::unique_ptr<EventBus>& event_bus);

	void OnProjectileHitsPlayer(entt::entity& projectile, entt::entity& player);
	void OnProjectileHitsEnemy(entt::entity& projectile, entt::entity& enemy);
	void OnCollision(CollisionEvent& event);
};


