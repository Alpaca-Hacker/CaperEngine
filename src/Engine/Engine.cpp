#include "Engine/Engine.h"

#include <iostream>
#include <SDL.h>

#include "AssetStore/AssetStore.h"
#include "Engine/LevelLoader.h"
#include "Events/KeypressEvent.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"
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

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_DockingEnable; //no viewports with SDL :(
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


void Engine::Setup()
{
	lua.open_libraries(sol::lib::base, sol::lib::math);
	LevelLoader loader;
	loader.LoadLevel(lua, 1, registry_, renderer_, asset_store_);

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
	if(event.symbol == SDLK_F1)
	{
		Logger::Log("Resetting level");
		registry_.clear();
		asset_store_->ClearAssets();
		LevelLoader loader;
		loader.LoadLevel(lua, 1, registry_, renderer_, asset_store_);
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