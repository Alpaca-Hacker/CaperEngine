#pragma once
#include "ECS/ECS.h"
#include "Events/CollisionEvent.h"
#include "Events/EventBus.h"

class DamageSystem : public System
{
public:
	DamageSystem();

	void SubscribeToEvents(std::unique_ptr<EventBus>& event_bus);

	void OnProjectileHitsPlayer(Entity& entity, Entity& entity1);
	void OnProjectileHitsEnemy(Entity& projectile, Entity& enemy);
	void OnCollision(CollisionEvent& event);
};


