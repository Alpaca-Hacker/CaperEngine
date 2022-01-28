#pragma once
#include <SDL.h>

#include "AssetStore/AssetStore.h"
#include "ECS/ECS.h"
#include "Events/EventBus.h"

const int FPS = 30;
constexpr int MILLISECS_PER_FRAME = 1000 / FPS;

class Engine
{
public:
	int window_width_;
	int window_height_;

	Engine();
	~Engine();

	void Initialise();
	void LoadLevel(int level);
	void Setup();
	void Run();
	void ProcessInput();
	void Update();
	void Render();
	void Shutdown();

private:

	SDL_Window* window_;
	SDL_Renderer* renderer_;
	bool is_running_;
	int millisecs_prev_frame_;
	bool display_hit_boxes_;

	std::unique_ptr<Registry> registry_;
	std::unique_ptr<AssetStore> asset_store_;
	std::unique_ptr<EventBus> event_bus_;
};
