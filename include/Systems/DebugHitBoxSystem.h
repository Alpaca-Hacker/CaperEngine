#pragma once
#include <SDL.h>

#include "Components/BoxColliderComponent.h"
#include "Components/TransformComponent.h"

class DebugHitBoxSystem 
{
public:

	void Update(const entt::registry& registry,SDL_Renderer* renderer, SDL_Rect camera)
	{
		for (auto entity : registry.view<BoxColliderComponent, TransformComponent>())
		{
			auto [transform, collider] = registry.get<TransformComponent, BoxColliderComponent>(entity);

			SDL_Rect rect{
				static_cast<int>((transform.position.x * transform.scale.x) + (collider.offset.x * transform.scale.x) - camera.x),
				static_cast<int>((transform.position.y * transform.scale.y) + (collider.offset.y * transform.scale.y) - camera.y),
				static_cast<int>(collider.width * transform.scale.x),
				static_cast<int>(collider.height * transform.scale.y)
			};

			if (collider.is_hit) {
				SDL_SetRenderDrawColor(renderer, 0xFF, 0x45, 0x00, 0xFF);
			}
			else {
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xD7, 0x00, 0xFF);
			}
			SDL_RenderDrawRect(renderer, &rect);
		}
	}
};