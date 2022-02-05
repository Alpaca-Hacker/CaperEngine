#include "Systems/ProjectileEmitterSystem.h"

#include "Components/BoxColliderComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/Tags.h"
#include "Components/TransformComponent.h"
#include "glm/gtx/rotate_vector.hpp"
#include <cmath>


void ProjectileEmitterSystem::Update(entt::registry& registry)
{
	for (auto entity : registry.view<ProjectileEmitterComponent, TransformComponent>())
	{
		auto [emitter, transform] = registry.get<ProjectileEmitterComponent, TransformComponent>(entity);


		if (SDL_GetTicks64() - emitter.last_emission_time > emitter.repeat_frequency && !registry.any_of<Player>(entity))
		{
			glm::vec2 position = transform.position;
			if (registry.any_of<SpriteComponent>(entity))
			{
				auto sprite = registry.get<SpriteComponent>(entity);
				position.x += (transform.scale.x * sprite.width / 2);
				position.y += (transform.scale.y * sprite.height / 2);
			}
			auto velocity = glm::vec2{static_cast<float>(cos(glm::radians(transform.rotation+90))),
				static_cast<float>(sin(glm::radians(transform.rotation+90)))
			};
			velocity *= emitter.projectile_speed;
			auto projectile = registry.create();
			registry.emplace<TransformComponent>(projectile, position);
			registry.emplace<RigidBodyComponent>(projectile, velocity);
			registry.emplace<SpriteComponent>(projectile, "bullet-image", 4, 4, 4);
			registry.emplace<BoxColliderComponent>(projectile, 4, 4, glm::vec2(0, 0));
			registry.emplace<ProjectileComponent>(projectile, emitter.is_friendly, emitter.hit_percent_damage, emitter.projectile_duration);
			registry.emplace<Projectile>(projectile);

			emitter.last_emission_time = SDL_GetTicks64();
		}
	}
}
