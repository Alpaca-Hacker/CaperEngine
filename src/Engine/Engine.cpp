#include "Engine/Engine.h"

#include <fstream>
#include <iostream>
#include <SDL.h>
#include <glm/glm.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"

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
#include "Components/TextComponent.h"
#include "Components/TransformComponent.h"
#include "Events/KeypressEvent.h"
#include "Log/Logger.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CameraMovementSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/DamageSystem.h"
#include "Systems/DebugHitBoxSystem.h"
#include "Systems/DebugSystem.h"
#include "Systems/HealthDisplaySystem.h"
#include "Systems/JanitorSystem.h"
#include "Systems/KeyboardControllerSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/ProjectileEmitterSystem.h"
#include "Systems/ProjectileLifecycleSystem.h"
#include "Systems/RenderImGuiSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/RenderTextSystem.h"

using namespace entt::literals;

int Engine::window_width_;
int Engine::window_height_;
int Engine::map_width_;
int Engine::map_height_;

Engine::Engine() : debug_mode_(false)
{
	asset_store_ = std::make_unique<AssetStore>();
	dispatcher_ = std::make_unique<entt::dispatcher>();
}

Engine::~Engine(){}

void Engine::Initialise()
{
	Logger::Initialise();

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cerr << "Error initializing SDL. Error:" << SDL_GetError() << std::endl;
		return;
	}

	if (TTF_Init() != 0)
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

	//Imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(window_);
	ImGui_ImplSDLRenderer_Init(renderer_);

	camera_.x = 0;
	camera_.y = 0;
	camera_.w = window_width_;
	camera_.h = window_height_;

	is_running_ = true;
}

void Engine::LoadLevel(int level)
{
	// TODO: Move to a resource cache
	asset_store_->AddTexture(renderer_, "tank-blue", "./assets/images/kenney/tank_blue.png");
	asset_store_->AddTexture(renderer_, "tank-dark", "./assets/images/kenney/tank_dark.png");
	asset_store_->AddTexture(renderer_, "tank-red", "./assets/images/kenney/tank_red.png");
	asset_store_->AddTexture(renderer_, "tank-sand", "./assets/images/kenney/tank_sand.png");
	asset_store_->AddTexture(renderer_, "tank-green", "./assets/images/kenney/tank_green.png");

	asset_store_->AddTexture(renderer_, "tree-green-large", "./assets/images/kenney/treeGreen_large.png");
	asset_store_->AddTexture(renderer_, "tree-green-small", "./assets/images/kenney/treeGreen_small.png");
	asset_store_->AddTexture(renderer_, "tree-brown-large", "./assets/images/kenney/treeBrown_large.png");
	asset_store_->AddTexture(renderer_, "tree-brown-small", "./assets/images/kenney/treeBrown_small.png");

	asset_store_->AddTexture(renderer_, "tilemap-image", "./assets/tilemaps/tanks.png");
	asset_store_->AddTexture(renderer_, "chopper-image", "./assets/images/chopper-spritesheet.png");
	asset_store_->AddTexture(renderer_, "radar-image", "./assets/images/radar.png");
	asset_store_->AddTexture(renderer_, "bullet-image", "./assets/images/bullet.png");
	asset_store_->AddFont("charriot-font", "./assets/fonts/charriot.ttf", 14);
	asset_store_->AddFont("covertops", "./assets/fonts/covertops.ttf", 25);

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
	registry_.emplace<ProjectileEmitterComponent>(chopper, -300, 300, 10000, 10, true);


	entt::entity radar = registry_.create();
	//radar.Group("ui");
	registry_.emplace<TransformComponent>(radar, glm::vec2(window_width_ - 74, 10), glm::vec2(1.0, 1.0), 0.0);
	registry_.emplace<RigidBodyComponent>(radar, glm::vec2(0.0, 0.0));
	registry_.emplace<SpriteComponent>(radar, "radar-image", 64, 64, 200, true);
	registry_.emplace<AnimationComponent>(radar, 8, 7, true);

	auto tank1 = registry_.create();
	auto tank2 = registry_.create();

	registry_.emplace<Enemy>(tank1);
	registry_.emplace<TransformComponent>(tank1, glm::vec2(500.0, 490.0), glm::vec2(1.0, 1.0), -90.0);
	registry_.emplace<RigidBodyComponent>(tank1, glm::vec2(50.0, 0.0));
	registry_.emplace<SpriteComponent>(tank1, "tank-blue", 42, 46, 2);
	registry_.emplace<BoxColliderComponent>(tank1, 42, 46);
	registry_.emplace<ProjectileEmitterComponent>(tank1, 100, 5000, 10000, 25, false);
	registry_.emplace<HealthComponent>(tank1, 100);

	//truck.Tag("truck");
	registry_.emplace<Enemy>(tank2);
	registry_.emplace<TransformComponent>(tank2, glm::vec2(105.0, 543.0), glm::vec2(1.0, 1.0), 0.0);
	registry_.emplace<RigidBodyComponent>(tank2, glm::vec2(0.0, 0.0));
	registry_.emplace<SpriteComponent>(tank2, "tank-dark", 42, 46, 1);
	registry_.emplace<BoxColliderComponent>(tank2, 42, 46);
	registry_.emplace<ProjectileEmitterComponent>(tank2, 100 , 2000, 6000, 10, false);
	registry_.emplace<HealthComponent>(tank2, 100);
	//registry_.emplace<DebugComponent>(truck);

	auto tree1 = registry_.create();
	registry_.emplace<Obstacle>(tree1);
	registry_.emplace<TransformComponent>(tree1, glm::vec2(1000.0, 482.0), glm::vec2(1.0, 1.0), 0.0);
	registry_.emplace<RigidBodyComponent>(tree1, glm::vec2(0.0, 0.0));
	registry_.emplace<SpriteComponent>(tree1, "tree-brown-large", 64, 64, 5);
	registry_.emplace<BoxColliderComponent>(tree1, 64, 64);

	auto tree2 = registry_.create();
	registry_.emplace<Obstacle>(tree2);
	registry_.emplace<TransformComponent>(tree2, glm::vec2(300.0, 482.0), glm::vec2(1.0, 1.0), 0.0);
	registry_.emplace<RigidBodyComponent>(tree2, glm::vec2(0.0, 0.0));
	registry_.emplace<SpriteComponent>(tree2, "tree-green-large", 64, 64, 5);
	registry_.emplace<BoxColliderComponent>(tree2, 64, 64);

	auto label =registry_.create();
	SDL_Colour cornflower_blue = { 0x64, 0x95, 0xED, 0xFF };
	registry_.emplace<TextComponent>(label, glm::vec2(window_width_ / 2 - 25, 25), "Chopper", "covertops", cornflower_blue, true);
}

void Engine::Setup()
{
	LoadLevel(1);

	dispatcher_->sink<KeypressEvent>().connect<&Engine::OnKeypress>(this);

	KeyboardControllerSystem keyboard_controller;
	keyboard_controller.SubscribeToEvents(dispatcher_);

	DamageSystem damage_system;
	damage_system.SubscribeToEvents(dispatcher_);

	MovementSystem movement_system;
	movement_system.SubscribeToEvents(dispatcher_);
}


void Engine::Run()
{
	Setup();
	while (is_running_)
	{
		ProcessEvents();
		Update();
		Render();
		OnEndOfFrame();
	}
}

void Engine::ProcessEvents()
{

	SDL_Event sdl_event;
	while (SDL_PollEvent(&sdl_event))
	{
		ImGui_ImplSDL2_ProcessEvent(&sdl_event);
		switch (sdl_event.type)
		{
		case SDL_QUIT:
			is_running_ = false;
			break;
		case SDL_KEYDOWN:
			dispatcher_->enqueue<KeypressEvent>(sdl_event.key.keysym.sym, &registry_);
			break;
		case SDL_KEYUP:
			dispatcher_->enqueue<KeyReleaseEvent>(sdl_event.key.keysym.sym, &registry_);
			break;
		default:
			break;
		}
	}
}

void Engine::OnKeypress(KeypressEvent& event)
{
	//Logger::Log("Key {} ({}) pressed!", event.symbol, SDL_GetKeyName(event.symbol));
	if (event.symbol == SDLK_ESCAPE)
	{
		is_running_ = false;
	}
	if (event.symbol == SDLK_F7)
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

	//Update Systems
	MovementSystem movement_system;
	movement_system.Update(registry_, delta_time);
	AnimationSystem anim_sys;
	anim_sys.Update(registry_);

	CollisionSystem collision_system;
	collision_system.Update(registry_, dispatcher_);
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
	RenderTextSystem render_text_system;
	render_text_system.Update(registry_, renderer_, asset_store_, camera_);
	HealthDisplaySystem health_display;
	health_display.Update(registry_, renderer_, asset_store_, camera_);

	if (debug_mode_) {
		DebugHitBoxSystem dhbs;
		dhbs.Update(registry_, renderer_, camera_);
		RenderImGuiSystem imgui;
		imgui.Update(registry_, renderer_, asset_store_, camera_);
	}
	SDL_RenderPresent(renderer_);
}

void Engine::OnEndOfFrame()
{
	dispatcher_->update();
	JanitorSystem janitor;
	janitor.Update(registry_);
}

void Engine::Shutdown()
{
	Logger::Log("Shutting down");
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();

	Logger::ShutDown();
}