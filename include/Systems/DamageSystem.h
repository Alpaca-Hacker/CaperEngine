#pragma once
#include "Events/CollisionEvent.h"

class DamageSystem 
{
public:

	void SubscribeToEvents(std::unique_ptr<entt::dispatcher>& dispatcher);

	void OnProjectileHitsPlayer(entt::entity& projectile, entt::entity& player, entt::registry* registry_);
	void OnProjectileHitsEnemy(entt::entity& projectile, entt::entity& enemy, entt::registry* registry);
	void OnCollision(CollisionEvent& event);
};


