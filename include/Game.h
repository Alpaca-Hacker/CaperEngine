#pragma once
#include <SDL.h>

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
};
