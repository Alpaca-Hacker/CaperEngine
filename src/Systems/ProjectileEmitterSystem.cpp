#include "Systems/ProjectileEmitterSystem.h"

#include "Components/BoxColliderComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/Tags.h"
#include "Components/TransformComponent.h"


void ProjectileEmitterSystem::Update(entt::registry& registry)
{
	for (auto entity : registry.view<ProjectileEmitterComponent, TransformComponent>())
	{
		auto [emitter, transform] = registry.get<ProjectileEmitterComponent, TransformComponent>(entity);

		if (SDL_GetTicks64() - emitter.last_emission_time > emitter.repeat_frequency)
		{
			glm::vec2 position = transform.position;
			if (registry.any_of<SpriteComponent>(entity))
			{
				auto sprite = registry.get<SpriteComponent>(entity);
				position.x += (transform.scale.x * sprite.width / 2);
				position.y += (transform.scale.y * sprite.height / 2);
			}
			auto projectile = registry.create();
			registry.emplace<TransformComponent>(projectile, position);
			registry.emplace<RigidBodyComponent>(projectile, emitter.projectile_velocity);
			registry.emplace<SpriteComponent>(projectile, "bullet-image", 4, 4, 4);
			registry.emplace<BoxColliderComponent>(projectile, 4, 4, glm::vec2(0, 0));
			registry.emplace<ProjectileComponent>(projectile, emitter.is_friendly, emitter.hit_percent_damage, emitter.projectile_duration);
			registry.emplace<Projectile>(projectile);

			emitter.last_emission_time = SDL_GetTicks64();
		}
	}
}
