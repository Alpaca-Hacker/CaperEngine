#include "Systems/ProjectileLifecycleSystem.h"

#include "Components/ProjectileComponent.h"
#include "Components/TransformComponent.h"
#include "Engine/Engine.h"

ProjectileLifecycleSystem::ProjectileLifecycleSystem()
{
	RequireComponent<ProjectileComponent>();
}

void ProjectileLifecycleSystem::Update()
{
	for (auto entity : GetSystemEntities())
	{
		auto& projectile = entity.GetComponent<ProjectileComponent>();

		if (SDL_GetTicks64() - projectile.start_time > projectile.duration)
		{
			entity.Kill();
		}
		else {
			if (entity.HasComponent<TransformComponent>())
			{
				const auto transform = entity.GetComponent<TransformComponent>();

				if (transform.position.x <= 0 || transform.position.x >= Engine::map_width_ - 5
					|| transform.position.y <= 0 || transform.position.y >= Engine::map_height_ - 5)
				{
					entity.Kill();
				}
			}
		}
	}
		
}
