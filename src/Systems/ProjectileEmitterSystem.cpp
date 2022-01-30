#include "Systems/ProjectileEmitterSystem.h"

#include "Components/BoxColliderComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"

ProjectileEmitterSystem::ProjectileEmitterSystem()
{
	RequireComponent<ProjectileEmitterComponent>();
	RequireComponent<TransformComponent>();
}

void ProjectileEmitterSystem::Update(std::unique_ptr<Registry>& registry)
{
	for (auto entity : GetSystemEntities())
	{
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& emitter = entity.GetComponent<ProjectileEmitterComponent>();

		if (SDL_GetTicks64() - emitter.last_emission_time > emitter.repeat_frequency)
		{
			glm::vec2 position = transform.position;
			if (entity.HasComponent<SpriteComponent>())
			{
				auto sprite = entity.GetComponent<SpriteComponent>();
				position.x += (transform.scale.x * sprite.width / 2);
				position.y += (transform.scale.y * sprite.height / 2);
			}
			Entity projectile = registry->CreateEntity();
			projectile.AddComponent<TransformComponent>(position);
			projectile.AddComponent<RigidBodyComponent>(emitter.projectile_velocity);
			projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
			projectile.AddComponent<BoxColliderComponent>( 4, 4, glm::vec2(0, 0));
			projectile.AddComponent<ProjectileComponent>(emitter.is_friendly, emitter.hit_percent_damage, emitter.projectile_duration);
			projectile.Group("projectile");

			emitter.last_emission_time = SDL_GetTicks64();
		}
	}
}
