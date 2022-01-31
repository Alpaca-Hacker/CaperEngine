#include "Systems/DamageSystem.h"

#include "Components/HealthComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/Tags.h"
#include "Events/CollisionEvent.h"
#include "Events/EventBus.h"


void DamageSystem::OnCollision(CollisionEvent& event)
{
	//Logger::Log("Boom! Entities {} and {} crashed!", event.a.GetId(), event.b.GetId());
	entt::entity a = event.a;
	entt::entity b = event.b;

	if (registry_.any_of<Projectile>(a)) {
		if (registry_.any_of<Player>(b)) {
			//hit player
			OnProjectileHitsPlayer(a, b);
		}
		else {
			if (registry_.any_of<Enemy>(b)) {
				//hit enemy
				OnProjectileHitsEnemy(a, b);

			}
		}
	}
	else {

		if (registry_.any_of<Projectile>(b)) {
			if (registry_.any_of<Player>(a)) {
				//hit player
				OnProjectileHitsPlayer(b, a);
			}
			else {
				if (registry_.any_of<Enemy>(a)) {
					//hit enemy
					OnProjectileHitsEnemy(b, a);
				}
			}
		}
	}
}

void DamageSystem::OnProjectileHitsPlayer(entt::entity& projectile, entt::entity& player)
{
	auto projectile_component = registry_.get<ProjectileComponent>(projectile);

	if (!projectile_component.is_friendly)
	{
		auto& health = registry_.get<HealthComponent>(player);
		health.health -= projectile_component.damage;

		if (health.health <=0)
		{
			registry_.emplace_or_replace<Destroy>(player);
		}
		registry_.emplace_or_replace<Destroy>(projectile);
	}
}

void DamageSystem::OnProjectileHitsEnemy(entt::entity& projectile, entt::entity& enemy)
{
	auto projectile_component = registry_.get<ProjectileComponent>(projectile);

	if (projectile_component.is_friendly)
	{
		auto& health = registry_.get<HealthComponent>(enemy);
		health.health -= projectile_component.damage;

		if (health.health <= 0)
		{
			registry_.emplace_or_replace<Destroy>(enemy);
		}
		registry_.emplace_or_replace<Destroy>(projectile);
	}
}

void DamageSystem::SubscribeToEvents(std::unique_ptr<EventBus>& event_bus)
{
	event_bus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
}