#include "Engine/LevelLoader.h"

#include "Components/AnimationComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/CameraFollowConponent.h"
#include "Components/HealthComponent.h"
#include "Components/KeyboardControlledComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/Tags.h"
#include "Components/TextComponent.h"
#include "Components/TransformComponent.h"
#include "Engine/Engine.h"
#include "glm/vec2.hpp"
#include <fstream>

#include "Log/Logger.h"
#include "sol/sol.hpp"


LevelLoader::LevelLoader()
{
}

LevelLoader::~LevelLoader()
{
}

void LevelLoader::LoadLevel(sol::state& lua, int level_number, entt::registry& registry, SDL_Renderer* renderer, std::unique_ptr<AssetStore>& asset_store)
{
	Logger::Log("Loading level {}", level_number);

	sol::load_result script = lua.load_file("./assets/scripts/level" + std::to_string(level_number) + ".lua");

	if (!script.valid())
	{
		sol::error err = script;
		std::string error_message = err.what();
		Logger::Err("Error in LUA: " + error_message);
		return;
	}

	lua.script_file("./assets/scripts/level" + std::to_string(level_number) + ".lua");

	sol::table level = lua["Level"];
	// Read assets
	//// TODO: Move to a resource cache

	sol::table assets = level["assets"];

	int i = 0;
	while (true)
	{
		sol::optional<sol::table> has_asset = assets[i];
		if (has_asset == sol::nullopt)
		{
			break;
		}
		auto asset = assets[i];
		std::string asset_type = asset["type"];
		std::string asset_id = asset["id"];
		std::string asset_file = asset["file"];
		if (asset_type == "texture")
		{
			asset_store->AddTexture(renderer, asset_id, asset_file);
		}
		if (asset_type == "font")
		{
			asset_store->AddFont(asset_id, asset_file, asset["size"]);
		}
		i++;
	}

	// Load the tilemap
	int tile_size = 64;
	double tile_scale = 1.0;
	int map_num_cols = 25;
	int map_num_rows = 20;

	std::fstream map_file;
	map_file.open("./assets/tilemaps/tanks.csv");

	for (int y = 0; y < map_num_rows; y++) {
		for (int x = 0; x < map_num_cols; x++) {
			char ch;
			map_file.get(ch);
			int srcRectY = std::atoi(&ch) * tile_size;
			map_file.get(ch);
			int srcRectX = std::atoi(&ch) * tile_size;
			map_file.ignore();

			auto tile = registry.create();
			registry.emplace<TransformComponent>(tile, glm::vec2(x * (tile_scale * tile_size), y * (tile_scale * tile_size)), glm::vec2(tile_scale, tile_scale), 0.0);
			registry.emplace<SpriteComponent>(tile, "tilemap-image", tile_size, tile_size, 0, false, srcRectX, srcRectY);
			registry.emplace<Tile>(tile);
		}
	}
	map_file.close();

	Engine::map_width_ = map_num_cols * tile_size * tile_scale;
	Engine::map_height_ = map_num_rows * tile_size * tile_scale;

	entt::entity chopper = registry.create();
	registry.emplace<Player>(chopper);
	registry.emplace<TransformComponent>(chopper, glm::vec2(100.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
	registry.emplace<RigidBodyComponent>(chopper, glm::vec2(0.0, 0.0));
	registry.emplace<SpriteComponent>(chopper, "chopper-image", 32, 32, 10);
	registry.emplace<AnimationComponent>(chopper, 2, 15, true);
	registry.emplace<BoxColliderComponent>(chopper, 32, 32);
	registry.emplace<KeyboardControlledComponent>(chopper, glm::vec2(0, -100), glm::vec2(100, 0), glm::vec2(0, 100), glm::vec2(-100, 0));
	registry.emplace<CameraFollowComponent>(chopper);
	registry.emplace<HealthComponent>(chopper, 100);
	registry.emplace<ProjectileEmitterComponent>(chopper, -300, 300, 10000, 10, true);


	entt::entity radar = registry.create();
	//radar.Group("ui");
	registry.emplace<TransformComponent>(radar, glm::vec2(Engine::window_width_ - 74, 10), glm::vec2(1.0, 1.0), 0.0);
	registry.emplace<RigidBodyComponent>(radar, glm::vec2(0.0, 0.0));
	registry.emplace<SpriteComponent>(radar, "radar-image", 64, 64, 200, true);
	registry.emplace<AnimationComponent>(radar, 8, 7, true);

	auto tank1 = registry.create();
	auto tank2 = registry.create();

	registry.emplace<Enemy>(tank1);
	registry.emplace<TransformComponent>(tank1, glm::vec2(500.0, 490.0), glm::vec2(1.0, 1.0), -90.0);
	registry.emplace<RigidBodyComponent>(tank1, glm::vec2(50.0, 0.0));
	registry.emplace<SpriteComponent>(tank1, "tank-blue", 42, 46, 2);
	registry.emplace<BoxColliderComponent>(tank1, 42, 46);
	registry.emplace<ProjectileEmitterComponent>(tank1, 100, 5000, 10000, 25, false);
	registry.emplace<HealthComponent>(tank1, 100);

	//truck.Tag("truck");
	registry.emplace<Enemy>(tank2);
	registry.emplace<TransformComponent>(tank2, glm::vec2(105.0, 543.0), glm::vec2(1.0, 1.0), 0.0);
	registry.emplace<RigidBodyComponent>(tank2, glm::vec2(0.0, 0.0));
	registry.emplace<SpriteComponent>(tank2, "tank-dark", 42, 46, 1);
	registry.emplace<BoxColliderComponent>(tank2, 42, 46);
	registry.emplace<ProjectileEmitterComponent>(tank2, 100, 2000, 6000, 10, false);
	registry.emplace<HealthComponent>(tank2, 100);
	//registry_.emplace<DebugComponent>(truck);

	auto tree1 = registry.create();
	registry.emplace<Obstacle>(tree1);
	registry.emplace<TransformComponent>(tree1, glm::vec2(1000.0, 482.0), glm::vec2(1.0, 1.0), 0.0);
	registry.emplace<RigidBodyComponent>(tree1, glm::vec2(0.0, 0.0));
	registry.emplace<SpriteComponent>(tree1, "tree-brown-large", 64, 64, 5);
	registry.emplace<BoxColliderComponent>(tree1, 64, 64);

	auto tree2 = registry.create();
	registry.emplace<Obstacle>(tree2);
	registry.emplace<TransformComponent>(tree2, glm::vec2(300.0, 482.0), glm::vec2(1.0, 1.0), 0.0);
	registry.emplace<RigidBodyComponent>(tree2, glm::vec2(0.0, 0.0));
	registry.emplace<SpriteComponent>(tree2, "tree-green-large", 64, 64, 5);
	registry.emplace<BoxColliderComponent>(tree2, 64, 64);

	auto label = registry.create();
	SDL_Colour cornflower_blue = { 0x64, 0x95, 0xED, 0xFF };
	registry.emplace<TextComponent>(label, glm::vec2(Engine::window_width_ / 2 - 25, 25), "Chopper", "covertops", cornflower_blue, true);
}
