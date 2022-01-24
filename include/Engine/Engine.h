#pragma once
#include <SDL.h>

#include "ECS/ECS.h"

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

	std::unique_ptr<Registry> registry_;
};
