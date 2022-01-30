#include "Systems/DamageSystem.h"

#include "Components/BoxColliderComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ProjectileComponent.h"
#include "Events/CollisionEvent.h"
#include "Events/EventBus.h"

DamageSystem::DamageSystem()
{
	RequireComponent<BoxColliderComponent>();
}

void DamageSystem::OnCollision(CollisionEvent& event)
{
	//Logger::Log("Boom! Entities {} and {} crashed!", event.a.GetId(), event.b.GetId());
	Entity a = event.a;
	Entity b = event.b;

	if (a.BelongsToGroup("projectile")) {
		if (b.HasTag("player")) {
			//hit player
			OnProjectileHitsPlayer(a, b);
		}
		if (b.BelongsToGroup("enemy")) {
			//hit enemy
			OnProjectileHitsEnemy(a, b);
		}
	}

	if (b.BelongsToGroup("projectile")) {
		if (a.HasTag("player")) {
			//hit player
			OnProjectileHitsPlayer(b, a);
		}
		if (a.BelongsToGroup("enemy")) {
			//hit enemy
			OnProjectileHitsEnemy(b, a);
		}
	}
}

void DamageSystem::OnProjectileHitsPlayer(Entity& projectile, Entity& player)
{
	auto projectile_component = projectile.GetComponent<ProjectileComponent>();

	if (!projectile_component.is_friendly)
	{
		auto& health = player.GetComponent<HealthComponent>();
		health.health -= projectile_component.damage;

		if (health.health <=0)
		{
			player.Kill();
		}
		projectile.Kill();
	}
}

void DamageSystem::OnProjectileHitsEnemy(Entity& projectile, Entity& enemy)
{
	auto projectile_component = projectile.GetComponent<ProjectileComponent>();

	if (projectile_component.is_friendly)
	{
		auto& health = enemy.GetComponent<HealthComponent>();
		health.health -= projectile_component.damage;

		if (health.health <= 0)
		{
			enemy.Kill();
		}
		projectile.Kill();
	}
}

void DamageSystem::SubscribeToEvents(std::unique_ptr<EventBus>& event_bus)
{
	event_bus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
}