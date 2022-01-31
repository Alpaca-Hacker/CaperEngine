#include "Engine/Engine.h"

#include <fstream>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>

#include "AssetStore/AssetStore.h"
#include "Components/AnimationComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/CameraFollowConponent.h"
#include "Components/DebugComponent.h"
#include "Components/HealthComponent.h"
#include "Components/KeyboardControlledComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/Tags.h"
#include "Components/TransformComponent.h"
#include "Events/KeypressEvent.h"
#include "Log/Logger.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CameraMovementSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/DamageSystem.h"
#include "Systems/DebugHitBoxSystem.h"
#include "Systems/DebugSystem.h"
#include "Systems/JanitorSystem.h"
#include "Systems/KeyboardControllerSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/ProjectileEmitterSystem.h"
#include "Systems/ProjectileLifecycleSystem.h"
#include "Systems/RenderSystem.h"

using namespace entt::literals;

int Engine::window_width_;
int Engine::window_height_;
int Engine::map_width_;
int Engine::map_height_;

Engine::Engine() : debug_mode_(false)
{
	//registry_ = std::make_unique <Registry>();
	asset_store_ = std::make_unique<AssetStore>();
	event_bus_ = std::make_unique<EventBus>();
}

Engine::~Engine()
{
}

void Engine::Initialise()
{
	Logger::Initialise();

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cerr << "Error initializing SDL. Error:" << SDL_GetError() << std::endl;
		return;
	}

	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	window_width_ = 1200; // display_mode.w;
	window_height_ = window_width_ / 16 * 9; // display_mode.h

	window_ = SDL_CreateWindow(
		nullptr,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width_,
		window_height_,
		SDL_WINDOW_BORDERLESS);
	if (!window_)
	{
		std::cerr << "Error initializing window. Error:" << SDL_GetError() << std::endl;
		return;
	}

	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer_)
	{
		std::cerr << "Error SDL renderer. Error:" << SDL_GetError() << std::endl;
		return;
	}
	//SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN);

	camera_.x = 0;
	camera_.y = 0;
	camera_.w = window_width_;
	camera_.h = window_height_;

	is_running_ = true;
}

void Engine::LoadLevel(int level)
{
	// TODO: Move to a resource cache
	asset_store_->AddTexture(renderer_, "tank-image", "./assets/images/kenney/Tank-right.png");
	asset_store_->AddTexture(renderer_, "truck-image", "./assets/images/kenney/Truck_Right.png");
	asset_store_->AddTexture(renderer_, "tilemap-image", "./assets/tilemaps/jungle.png");
	asset_store_->AddTexture(renderer_, "chopper-image", "./assets/images/chopper-spritesheet.png");
	asset_store_->AddTexture(renderer_, "radar-image", "./assets/images/radar.png");
	asset_store_->AddTexture(renderer_, "bullet-image", "./assets/images/bullet.png");

	// Load the tilemap
	int tile_size = 32;
	double tile_scale = 2.0;
	int map_num_cols = 25;
	int map_num_rows = 20;

	std::fstream map_file;
	map_file.open("./assets/tilemaps/jungle.map");

	for (int y = 0; y < map_num_rows; y++) {
		for (int x = 0; x < map_num_cols; x++) {
			char ch;
			map_file.get(ch);
			int srcRectY = std::atoi(&ch) * tile_size;
			map_file.get(ch);
			int srcRectX = std::atoi(&ch) * tile_size;
			map_file.ignore();

			auto tile = registry_.create();
			registry_.emplace<TransformComponent>(tile, glm::vec2(x * (tile_scale * tile_size), y * (tile_scale * tile_size)), glm::vec2(tile_scale, tile_scale), 0.0);
			registry_.emplace<SpriteComponent>(tile, "tilemap-image", tile_size, tile_size, 0, false, srcRectX, srcRectY);
			registry_.emplace<Tile>(tile);
		}
	}
	map_file.close();

	map_width_ = map_num_cols * tile_size * tile_scale;
	map_height_ = map_num_rows * tile_size * tile_scale;

	entt::entity chopper = registry_.create();
	registry_.emplace<Player>(chopper);
	registry_.emplace<TransformComponent>(chopper, glm::vec2(100.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
	registry_.emplace<RigidBodyComponent>(chopper, glm::vec2(0.0, 0.0));
	registry_.emplace<SpriteComponent>(chopper, "chopper-image", 32, 32, 10);
	registry_.emplace<AnimationComponent>(chopper, 2, 15, true);
	registry_.emplace<BoxColliderComponent>(chopper, 32, 32);
	registry_.emplace<KeyboardControlledComponent>(chopper, glm::vec2(0, -100), glm::vec2(100, 0), glm::vec2(0, 100), glm::vec2(-100, 0));
	registry_.emplace<CameraFollowComponent>(chopper);
	registry_.emplace<HealthComponent>(chopper, 100);
	registry_.emplace<ProjectileEmitterComponent>(chopper, glm::vec2(0.0, -300.0), 1000000, 10000, 10, true);


	entt::entity radar = registry_.create();
	//radar.Group("ui");
	registry_.emplace<TransformComponent>(radar, glm::vec2(window_width_ - 74, 10), glm::vec2(1.0, 1.0), 0.0);
	registry_.emplace<RigidBodyComponent>(radar, glm::vec2(0.0, 0.0));
	registry_.emplace<SpriteComponent>(radar, "radar-image", 64, 64, 200, true);
	registry_.emplace<AnimationComponent>(radar, 8, 7, true);

	auto tank = registry_.create();
	auto truck = registry_.create();

	registry_.emplace<Enemy>(tank);
	registry_.emplace<TransformComponent>(tank, glm::vec2(500.0, 482.0), glm::vec2(1.0, 1.0), 0.0);
	registry_.emplace<RigidBodyComponent>(tank, glm::vec2(0.0, 0.0));
	registry_.emplace<SpriteComponent>(tank, "tank-image", 64, 64, 2);
	registry_.emplace<BoxColliderComponent>(tank, 64, 64);
	registry_.emplace<ProjectileEmitterComponent>(tank, glm::vec2(100.0, 0.0), 5000, 10000, 25, false);
	registry_.emplace<HealthComponent>(tank, 100);

	//truck.Tag("truck");
	registry_.emplace<Enemy>(truck);
	registry_.emplace<TransformComponent>(truck, glm::vec2(100.0, 482.0), glm::vec2(1.0, 1.0), 0.0);
	registry_.emplace<RigidBodyComponent>(truck, glm::vec2(0.0, 0.0));
	registry_.emplace<SpriteComponent>(truck, "truck-image", 64, 64, 1);
	registry_.emplace<BoxColliderComponent>(truck, 64, 64);
	registry_.emplace<ProjectileEmitterComponent>(truck,glm::vec2(0.0, 100.0), 2000, 6000, 10, false);
	registry_.emplace<HealthComponent>(truck, 100);
	registry_.emplace<DebugComponent>(truck);
}

void Engine::Setup()
{
	LoadLevel(1);
}


void Engine::Run()
{
	Setup();
	while (is_running_)
	{
		ProcessInput();
		Update();
		Render();
		OnEndOfFrame();
	}
}

void Engine::ProcessInput()
{
	SDL_Event sdl_event;
	while (SDL_PollEvent(&sdl_event))
	{
		switch (sdl_event.type)
		{
		case SDL_QUIT:
			is_running_ = false;
			break;
		case SDL_KEYDOWN:
			event_bus_->EmitEvent<KeypressEvent>(sdl_event.key.keysym.sym, registry_);
			break;
		case SDL_KEYUP:
			event_bus_->EmitEvent<KeyReleaseEvent>(sdl_event.key.keysym.sym, registry_);
			break;
		default:
			break;
		}
	}
}

void Engine::OnKeypress(KeypressEvent& event)
{

	if (event.symbol_ == SDLK_ESCAPE)
	{
		is_running_ = false;
	}
	if (event.symbol_ == SDLK_F7)
	{
		debug_mode_ = !debug_mode_;
	}
}

void Engine::Update()
{
	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks64() - millisecs_prev_frame_);
	if (timeToWait > 0 && timeToWait <= millisecs_prev_frame_)
	{
		SDL_Delay(timeToWait);
	}

	double delta_time = (SDL_GetTicks64() - millisecs_prev_frame_) / 1000.0f;

	millisecs_prev_frame_ = SDL_GetTicks64();

	event_bus_->Reset();

	//Subscribe to Events for all systems
	event_bus_->SubscribeToEvent<KeypressEvent>(this, &Engine::OnKeypress);
	KeyboardControllerSystem keyboard_controller;

	keyboard_controller.SubscribeToEvents(event_bus_);
	DamageSystem damage_system(registry_);
	damage_system.SubscribeToEvents(event_bus_);
	
	//Update Systems
	MovementSystem movement_system;
	movement_system.Update(registry_, delta_time);
	AnimationSystem anim_sys;
	anim_sys.Update(registry_);

	CollisionSystem collision_system;
	collision_system.Update(registry_, event_bus_);
	CameraMovementSystem cms;
	cms.Update(registry_, camera_);
	ProjectileEmitterSystem pes;
	pes.Update(registry_);
	ProjectileLifecycleSystem projectile_lifecycle;
	projectile_lifecycle.Update(registry_);

	if (debug_mode_)
	{
		DebugSystem debug;
		debug.Update(registry_);
	}
}

void Engine::Render()
{
	SDL_SetRenderDrawColor(renderer_, 0x15, 0x15, 0x15, 0xFF);
	SDL_RenderClear(renderer_);
	RenderSystem render_system;
	render_system.Update(registry_, renderer_, asset_store_, camera_);

	if (debug_mode_) {
		DebugHitBoxSystem dhbs;
		dhbs.Update(registry_, renderer_, camera_);
	}
	SDL_RenderPresent(renderer_);
}

void Engine::OnEndOfFrame()
{
	JanitorSystem janitor;
	janitor.Update(registry_);
}

void Engine::Shutdown()
{
	Logger::Log("Shutting down");

	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();

	Logger::ShutDown();
}