#include "Systems/DamageSystem.h"

#include "Components/HealthComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/Tags.h"
#include "Events/CollisionEvent.h"


void DamageSystem::OnCollision(CollisionEvent& event)
{
	//Logger::Log("Boom! Entities {} and {} crashed!", event.a.GetId(), event.b.GetId());

	if (!event.registry->valid(event.a) || !event.registry->valid(event.b))
	{
		return;
	}

	entt::entity a = event.a;
	entt::entity b = event.b;

	if (event.registry->any_of<Projectile>(a)) {
		if (event.registry->any_of<Player>(b)) {
			//hit player
			OnProjectileHitsPlayer(a, b, event.registry);
		}
		else {
			if (event.registry->any_of<Enemy>(b)) {
				//hit enemy
				OnProjectileHitsEnemy(a, b, event.registry);
			}
		}
	}
	else {

		if (event.registry->any_of<Projectile>(b)) {
			if (event.registry->any_of<Player>(a)) {
				//hit player
				OnProjectileHitsPlayer(b, a, event.registry);
			}
			else {
				if (event.registry->any_of<Enemy>(a)) {
					//hit enemy
					OnProjectileHitsEnemy(b, a, event.registry);
				}
			}
		}
	}
}

void DamageSystem::OnProjectileHitsPlayer(entt::entity& projectile, entt::entity& player, entt::registry* registry_)
{
	auto projectile_component = registry_->get<ProjectileComponent>(projectile);

	if (!projectile_component.is_friendly)
	{
		auto& health = registry_->get<HealthComponent>(player);
		health.health -= projectile_component.damage;

		if (health.health <=0)
		{
			registry_->emplace_or_replace<Destroy>(player);
		}
		registry_->emplace_or_replace<Destroy>(projectile);
	}
}

void DamageSystem::OnProjectileHitsEnemy(entt::entity& projectile, entt::entity& enemy, entt::registry* registry_)
{
	auto projectile_component = registry_->get<ProjectileComponent>(projectile);

	if (projectile_component.is_friendly)
	{
		auto& health = registry_->get<HealthComponent>(enemy);
		health.health -= projectile_component.damage;

		if (health.health <= 0)
		{
			registry_->emplace_or_replace<Destroy>(enemy);
		}
		registry_->emplace_or_replace<Destroy>(projectile);
	}
}

void DamageSystem::SubscribeToEvents(std::unique_ptr<entt::dispatcher>& dispatcher)
{
	dispatcher->sink<CollisionEvent>().connect<&DamageSystem::OnCollision>(this);
}