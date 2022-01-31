#pragma once
#include <SDL.h>
#include <entt/entt.hpp>

#include "AssetStore/AssetStore.h"

class RenderSystem 
{
public:
	void Update(const entt::registry& registry,SDL_Renderer* renderer, const std::unique_ptr<AssetStore>& asset_store, SDL_Rect camera);
};
