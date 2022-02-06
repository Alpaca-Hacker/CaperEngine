#pragma once
#include <SDL.h>
#include <entt/entt.hpp>

#include "AssetStore/AssetStore.h"
#include "Events/KeypressEvent.h"
#include "sol/sol.hpp"

const int FPS = 30;
constexpr int MILLISECS_PER_FRAME = 1000 / FPS;

class Engine
{
public:
	static int window_width_;
	static int window_height_;
	static int map_width_;
	static int map_height_;

	Engine();
	~Engine();

	void Initialise();
	void Setup();
	void OnEndOfFrame();
	void Run();
	void ProcessEvents();
	void OnKeypress(KeypressEvent& event);
	void Update();
	void Render();
	void Shutdown();

private:

	sol::state lua; // Needs to be at the top else stuff happens
	SDL_Window* window_;
	SDL_Renderer* renderer_;
	bool is_running_;
	int millisecs_prev_frame_;
	bool debug_mode_;
	SDL_Rect camera_;

	entt::registry registry_;
	std::unique_ptr<AssetStore> asset_store_;
	std::unique_ptr<entt::dispatcher> dispatcher_;


};
