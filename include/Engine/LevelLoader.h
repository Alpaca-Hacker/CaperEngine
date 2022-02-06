#pragma once
#include <memory>
#include <entt/entt.hpp>

#include "AssetStore/AssetStore.h"
#include "sol/sol.hpp"

class LevelLoader
{
public:
	LevelLoader();
	~LevelLoader();

	void LoadLevel(sol::state& lua, int level_number, entt::registry& registry, SDL_Renderer* renderer, std::unique_ptr<AssetStore>& asset_store);
};
