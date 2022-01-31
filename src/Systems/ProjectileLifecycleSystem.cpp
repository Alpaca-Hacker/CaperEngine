#include "Systems/ProjectileLifecycleSystem.h"

#include "Components/ProjectileComponent.h"
#include "Components/TransformComponent.h"
#include "Engine/Engine.h"


void ProjectileLifecycleSystem::Update(entt::registry& registry)
{
	for (auto entity : registry.view<ProjectileComponent>())
	{
		auto& projectile = registry.get<ProjectileComponent>(entity);

		if (SDL_GetTicks64() - projectile.start_time > projectile.duration)
		{
			registry.destroy(entity);
		}
		else {
			if (registry.any_of<TransformComponent>(entity))
			{
				const auto transform = registry.get<TransformComponent>(entity);

				if (transform.position.x <= 0 || transform.position.x >= Engine::map_width_ - 5
					|| transform.position.y <= 0 || transform.position.y >= Engine::map_height_ - 5)
				{
					registry.destroy(entity);
				}
			}
		}
	}
}
