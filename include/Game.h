#pragma once
#include <SDL.h>

const int FPS = 30;
constexpr int MILLISECS_PER_FRAME = 1000 / FPS;

class Game
{
public:
	int window_width_;
	int window_height_;

	Game();
	~Game();

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
	SDL_Texture* texture_;
	int millisecs_prev_frame_;
};
