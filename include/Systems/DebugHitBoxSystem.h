#pragma once
#include <SDL.h>

#include "Components/BoxColliderComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"

class DebugHitBoxSystem : public System
{
public:
	DebugHitBoxSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<BoxColliderComponent>();
	}

	void Update(SDL_Renderer* renderer)
	{
		for (auto entity : GetSystemEntities())
		{
			const auto& transform = entity.GetComponent<TransformComponent>();
			const auto& collider = entity.GetComponent<BoxColliderComponent>();

			SDL_Rect rect{
				static_cast<int>((transform.position.x * transform.scale.x) + (collider.offset.x * transform.scale.x)),
				static_cast<int>((transform.position.y * transform.scale.y) + (collider.offset.y * transform.scale.y)),
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