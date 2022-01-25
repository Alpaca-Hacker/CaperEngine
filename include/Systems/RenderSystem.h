#pragma once
#include <SDL.h>

#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"

class RenderSystem : public System
{
public:
	RenderSystem()
	{
		RequriedComponent<TransformComponent>();
		RequriedComponent<SpriteComponent>();
	}

	void Update(SDL_Renderer* renderer)
	{
		for (auto entity : GetSystemEntities())
		{
			const auto& transform = entity.GetComponent<TransformComponent>();
			const auto& sprite = entity.GetComponent<SpriteComponent>();

			SDL_Rect obj_rect{
				static_cast<int>(transform.position.x),
				static_cast<int>(transform.position.y),
				sprite.width,
				sprite.height };

			SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
			SDL_RenderFillRect(renderer, &obj_rect);

			Logger::Log("Entity {} now at position [{}, {}]", entity.GetId(), transform.position.x, transform.position.y);
		}
	}
};
