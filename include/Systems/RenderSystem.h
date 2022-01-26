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

	void Update(SDL_Renderer* renderer, const std::unique_ptr<AssetStore>& asset_store)
	{
		for (auto entity : GetSystemEntities())
		{
			const auto& transform = entity.GetComponent<TransformComponent>();
			const auto& sprite = entity.GetComponent<SpriteComponent>();

			SDL_Rect src_rect = sprite.src_rect;

			SDL_Rect dest_rect{
	static_cast<int>(transform.position.x),
	static_cast<int>(transform.position.y),
	static_cast<int>(static_cast<float>(sprite.width) * transform.scale.x),
	static_cast<int>(static_cast<float>(sprite.height) * transform.scale.y)
			};

			SDL_RenderCopyEx(
				renderer,
				asset_store->GetTexture(sprite.asset_id),
				&src_rect,
				&dest_rect,
				transform.rotation,
				nullptr,
				SDL_FLIP_NONE
			);
		}
	}
};
