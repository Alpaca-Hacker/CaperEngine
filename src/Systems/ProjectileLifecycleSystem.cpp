#include "Systems/ProjectileLifecycleSystem.h"

#include "Components/ProjectileComponent.h"

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
	}
		
}
