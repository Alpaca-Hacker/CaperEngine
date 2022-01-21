#include "Game.h"

#include <iostream>
#include <SDL.h>

Game::Game()
{
}

Game::~Game()
{
}

void Game::Initialise()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cerr << "Error initializing SDL. Error:" << SDL_GetError() << std::endl;
		return;
	}

	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	window_width_ = 800; // display_mode.w;
	window_height_ = 600; // display_mode.h

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
	SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN);

	is_running_ = true;
}

void Game::Setup()
{
}

void Game::Run()
{
	Setup();
	while (is_running_)
	{
		ProcessInput();
		Update();
		Render();
	}
}

void Game::ProcessInput()
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
			if (sdl_event.key.keysym.sym == SDLK_ESCAPE)
			{
				is_running_ = false;
			}
			break;
		default:
			break;
		}
	}
}

void Game::Update()
{
}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer_, 0x15, 0x15, 0x15, 0xFF);
	SDL_RenderClear(renderer_);

	SDL_SetRenderDrawColor(renderer_, 0x64, 0x95, 0xED, 0xFF);
	SDL_Rect player = { 10, 10, 20, 20 };
	SDL_RenderFillRect(renderer_, &player);

	SDL_RenderPresent(renderer_);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();
}