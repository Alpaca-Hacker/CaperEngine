#pragma once
#include <entt/entity/registry.hpp>

#include "AssetStore/AssetStore.h"

class RenderTextSystem
{
public:
	void Update(entt::registry& registry, SDL_Renderer* renderer, std::unique_ptr<AssetStore>& asset_store, SDL_Rect& camera);
};

