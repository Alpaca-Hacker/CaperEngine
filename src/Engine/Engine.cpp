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
#include "Components/TransformComponent.h"
#include "Events/KeypressEvent.h"
#include "Log/Logger.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CameraMovementSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/DamageSystem.h"
#include "Systems/DebugHitBoxSystem.h"
#include "Systems/DebugSystem.h"
#include "Systems/KeyboardControllerSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/ProjectileEmitterSystem.h"
#include "Systems/ProjectileLifecycleSystem.h"
#include "Systems/RenderSystem.h"

int Engine::window_width_;
int Engine::window_height_;
int Engine::map_width_;
int Engine::map_height_;

Engine::Engine() : display_hit_boxes_(false)
{
	registry_ = std::make_unique <Registry>();
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
	//Add Systems
	registry_->AddSystem<MovementSystem>();
	registry_->AddSystem<RenderSystem>();
	registry_->AddSystem<DebugHitBoxSystem>();
	registry_->AddSystem<AnimationSystem>();
	registry_->AddSystem<CollisionSystem>();
	registry_->AddSystem<DamageSystem>();
	registry_->AddSystem<KeyboardControllerSystem>();
	registry_->AddSystem<CameraMovementSystem>();
	registry_->AddSystem<DebugSystem>();
	registry_->AddSystem<ProjectileEmitterSystem>();
	registry_->AddSystem<ProjectileLifecycleSystem>();

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

			Entity tile = registry_->CreateEntity();
			tile.AddComponent<TransformComponent>(glm::vec2(x * (tile_scale * tile_size), y * (tile_scale * tile_size)), glm::vec2(tile_scale, tile_scale), 0.0);
			tile.AddComponent<SpriteComponent>("tilemap-image", tile_size, tile_size, 0, false, srcRectX, srcRectY);
			tile.Group("tile");
		}
	}
	map_file.close();

	map_width_ = map_num_cols * tile_size * tile_scale;
	map_height_ = map_num_rows * tile_size * tile_scale;

	Entity chopper = registry_->CreateEntity();
	chopper.Tag("player");
	chopper.AddComponent<TransformComponent>(glm::vec2(100.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 10);
	chopper.AddComponent<AnimationComponent>(2, 15, true);
	chopper.AddComponent<BoxColliderComponent>(32, 32);
	chopper.AddComponent<KeyboardControlledComponent>(glm::vec2(0, -100), glm::vec2(100, 0), glm::vec2(0, 100), glm::vec2(-100, 0));
	chopper.AddComponent<CameraFollowComponent>();
	chopper.AddComponent<HealthComponent>(100);
	chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0, -300.0), 1000000, 10000, 10, true);


	Entity radar = registry_->CreateEntity();
	radar.Tag("radar");
	radar.Group("ui");
	radar.AddComponent<TransformComponent>(glm::vec2(window_width_ - 74, 10), glm::vec2(1.0, 1.0), 0.0);
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 200, true);
	radar.AddComponent<AnimationComponent>(8, 7, true);

	Entity tank = registry_->CreateEntity();
	Entity truck = registry_->CreateEntity();

	tank.Tag("tank");
	tank.Group("enemy");
	tank.AddComponent<TransformComponent>(glm::vec2(500.0, 482.0), glm::vec2(1.0, 1.0), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	tank.AddComponent<SpriteComponent>("tank-image", 64, 64, 2);
	tank.AddComponent<BoxColliderComponent>(64, 64);
	tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 5000, 10000, 25, false);
	tank.AddComponent<HealthComponent>(100);

	truck.Tag("truck");
	truck.Group("enemy");
	truck.AddComponent<TransformComponent>(glm::vec2(100.0, 482.0), glm::vec2(1.0, 1.0), 0.0);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	truck.AddComponent<SpriteComponent>("truck-image", 64, 64, 1);
	truck.AddComponent<BoxColliderComponent>(64, 64);
	truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0, 100.0), 2000, 6000, 10, false);
	truck.AddComponent<HealthComponent>(100);
	//truck.AddComponent<DebugComponent>();
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
			event_bus_->EmitEvent<KeypressEvent>(sdl_event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			event_bus_->EmitEvent<KeyReleaseEvent>(sdl_event.key.keysym.sym);
			break;
		default:
			break;
		}
	}
}

void Engine::OnKeypress(KeypressEvent& event)
{

	if (event.symbol == SDLK_ESCAPE)
	{
		is_running_ = false;
	}
	if (event.symbol == SDLK_F7)
	{
		display_hit_boxes_ = !display_hit_boxes_;
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
	registry_->GetSystem<KeyboardControllerSystem>().SubscribeToEvents(event_bus_);
	registry_->GetSystem<DamageSystem>().SubscribeToEvents(event_bus_);

	//Update Systems
	registry_->GetSystem<MovementSystem>().Update(delta_time);
	registry_->GetSystem<AnimationSystem>().Update();
	registry_->GetSystem<CollisionSystem>().Update(event_bus_);
	registry_->GetSystem<CameraMovementSystem>().Update(camera_);
	registry_->GetSystem<DebugSystem>().Update();
	registry_->GetSystem<ProjectileEmitterSystem>().Update(registry_);
	registry_->GetSystem<ProjectileLifecycleSystem>().Update();

	//Update Registry
	registry_->Update();
}

void Engine::Render()
{
	SDL_SetRenderDrawColor(renderer_, 0x15, 0x15, 0x15, 0xFF);
	SDL_RenderClear(renderer_);
	registry_->GetSystem<RenderSystem>().Update(renderer_, asset_store_, camera_);

	if (display_hit_boxes_) {
		registry_->GetSystem<DebugHitBoxSystem>().Update(renderer_, camera_);
	}
	SDL_RenderPresent(renderer_);
}

void Engine::Shutdown()
{
	Logger::Log("Shutting down");

	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();

	Logger::ShutDown();
}