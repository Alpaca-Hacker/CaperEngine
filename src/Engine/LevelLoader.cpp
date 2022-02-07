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

	Engine::map_width_ = Engine::window_width_;
	Engine::map_height_ = Engine::window_height_;

	sol::optional<sol::table> has_map = level["tilemap"];

	if (has_map != sol::nullopt)
	{
		sol::table tilemap = level["tilemap"];
		// Load the tilemap
		int tile_size = tilemap["tile_size"];
		double tile_scale = tilemap["tile_scale"];
		int map_num_cols = tilemap["num_cols"];
		int map_num_rows = tilemap["num_rows"];
		std::string map_asset_id = tilemap["asset_id"];

		std::fstream map_file;
		std::string file = tilemap["map_file"];
		map_file.open(file);

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
				registry.emplace<SpriteComponent>(tile, map_asset_id, tile_size, tile_size, 0, false, srcRectX, srcRectY);
				registry.emplace<Tile>(tile);
			}
		}
		map_file.close();

		Engine::map_width_ = map_num_cols * tile_size * tile_scale;
		Engine::map_height_ = map_num_rows * tile_size * tile_scale;
	}

	sol::optional<sol::table> has_entities = level["entities"];
	if (has_entities == sol::nullopt)
	{
		return; //Nothing else to check
	}

	sol::table entities = level["entities"];
	i = 0;
	while (true)
	{
		Logger::Log("Loading Entity {}", i);
		sol::optional<sol::table> has_entity = entities[i];
		if (has_entity == sol::nullopt)
		{
			break;
		}
		auto entity = entities[i];
		entt::entity entity_to_add = registry.create();
		sol::optional<std::string> has_tag = entity["tag"];
		if (has_tag != sol::nullopt)
		{
			std::string tag = entity["tag"];
			if (tag == "Player")
			{
				registry.emplace<Player>(entity_to_add);
			}
			if (tag == "Enemy")
			{
				registry.emplace<Enemy>(entity_to_add);
			}
			if (tag == "Obstacle")
			{
				registry.emplace<Obstacle>(entity_to_add);
			}
		}

		sol::optional<sol::table> has_components = entity["components"];
		if (has_components != sol::nullopt)
		{
			sol::table components = entity["components"];
			if (sol::optional<sol::table> has_component = components["transform"]; has_component != sol::nullopt)
			{
				sol::table transform = components["transform"];
				registry.emplace<TransformComponent>(
					entity_to_add,
					glm::vec2(transform["position"]["x"], transform["position"]["y"]),
					glm::vec2(transform["scale"]["x"], transform["scale"]["y"]),
					transform["rotation"]);
			}

			if (sol::optional<sol::table> has_component = components["rigidbody"]; has_component != sol::nullopt)
			{
				sol::table rigidbody = components["rigidbody"];
				registry.emplace<RigidBodyComponent>(
					entity_to_add,
					glm::vec2(rigidbody["velocity"]["x"], rigidbody["velocity"]["y"])
					);
			}

			if (sol::optional<sol::table> has_component = components["sprite"]; has_component != sol::nullopt)
			{
				sol::table sprite = components["sprite"];
				registry.emplace<SpriteComponent>(
					entity_to_add,
					sprite["texture_asset_id"],
					sprite["width"],
					sprite["height"],
					sprite["z_index"].get_or(1),
					sprite["is_fixed"].get_or(false),
					sprite["src_rect_x"].get_or(0),
					sprite["src_rect_y"].get_or(0)
					);
			}

			if (sol::optional<sol::table> has_component = components["animation"]; has_component != sol::nullopt)
			{
				sol::table animation = components["animation"];
				registry.emplace<AnimationComponent>(
					entity_to_add,
					animation["num_frames"],
					animation["speed_rate"],
					animation["is_loop"].get_or(true));
			}

			if (sol::optional<sol::table> has_component = components["boxcollider"]; has_component != sol::nullopt)
			{
				sol::table boxcollider = components["boxcollider"];
				registry.emplace<BoxColliderComponent>(
					entity_to_add,
					boxcollider["width"],
					boxcollider["height"],
					glm::vec2(boxcollider["offset"]["x"].get_or(0.0), boxcollider["offset"]["y"].get_or(0.0)));
			}

			//if (sol::optional<int> has_component = components["health"]; has_component != sol::nullopt)
			//{
			//	int health = components["health"].get_or(100);
			//	registry.emplace<HealthComponent>(entity_to_add, health);
			//}
			if (sol::optional<sol::table> has_component = components["health"]; has_component != sol::nullopt)
			{
				int health = components["health"]["health_percentage"].get_or(100);
				registry.emplace<HealthComponent>(entity_to_add, health);
			}

			if (sol::optional<sol::table> has_component = components["projectile_emitter"]; has_component != sol::nullopt)
			{
				sol::table projectile_emitter = components["projectile_emitter"];

				registry.emplace<ProjectileEmitterComponent>(
					entity_to_add,
					projectile_emitter["projectile_velocity"]["x"],
					static_cast<int> (projectile_emitter["repeat_frequency"]) * 1000,
					static_cast<int> (projectile_emitter["projectile_duration"]) * 1000,
					projectile_emitter["hit_percentage_damage"],
					projectile_emitter["friendly"]
					);
			}

			if (sol::optional<sol::table> has_component = components["keyboard_controlled"]; has_component != sol::nullopt)
			{
				sol::table keyboard_controlled = components["keyboard_controlled"];
				registry.emplace<KeyboardControlledComponent>(
					entity_to_add,
					glm::vec2(keyboard_controlled["up_velocity"]["x"], keyboard_controlled["up_velocity"]["y"]),
					glm::vec2(keyboard_controlled["right_velocity"]["x"], keyboard_controlled["right_velocity"]["y"]),
					glm::vec2(keyboard_controlled["down_velocity"]["x"], keyboard_controlled["down_velocity"]["y"]),
					glm::vec2(keyboard_controlled["left_velocity"]["x"], keyboard_controlled["left_velocity"]["y"])
					);
			}

			if (sol::optional<sol::table> has_component = components["camera_follow"]; has_component != sol::nullopt)
			{
				registry.emplace<CameraFollowComponent>(entity_to_add);
			}

			if (sol::optional<sol::table> has_component = components["text"]; has_component != sol::nullopt)
			{
				sol::table text_component = components["text"];
				SDL_Colour colour = { text_component["colour"]["red"],text_component["colour"]["green"],
					text_component["colour"]["blue"],text_component["colour"]["alpha"] };
				registry.emplace<TextComponent>(
					entity_to_add,
					glm::vec2(text_component["position"]["x"], text_component["position"]["y"]),
					text_component["text"],
					text_component["font"],
					colour,
					text_component["is_fixed"]
					);
			}
		}

		i++;
	}

}
