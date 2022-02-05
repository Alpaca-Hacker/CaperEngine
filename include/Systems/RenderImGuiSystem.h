#pragma once
#include <SDL_render.h>
#include <entt/entity/registry.hpp>

#include "AssetStore/AssetStore.h"

class RenderImGuiSystem
{
public:
	RenderImGuiSystem() = default;
	void Update(entt::registry& registry, SDL_Renderer* renderer, std::unique_ptr<AssetStore>& asset_store, SDL_Rect camera);
};


