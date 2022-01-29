#pragma once
#include <SDL.h>

#include "AssetStore/AssetStore.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"

class RenderSystem : public System
{
public:
	RenderSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(SDL_Renderer* renderer, const std::unique_ptr<AssetStore>& asset_store, SDL_Rect camera);
};
