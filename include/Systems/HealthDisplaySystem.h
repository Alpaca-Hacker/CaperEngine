#pragma once
#include <SDL.h>
#include <entt/entt.hpp>

#include "AssetStore/AssetStore.h"

class HealthDisplaySystem
{
public:
	void Update(entt::registry& registry, SDL_Renderer* renderer, std::unique_ptr<AssetStore>& asset_store, SDL_Rect& camera);
};
