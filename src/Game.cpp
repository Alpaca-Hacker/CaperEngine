#include "Game.h"

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>

#include "Logger.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Initialise()
{
	Logger::Initialise();

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
	//SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN);

	is_running_ = true;
}

glm::vec2 playerPosition;
glm::vec2 playerVelocity;

void Game::Setup()
{
	SDL_Surface* surface = IMG_Load("./assets/images/tank-tiger-right.png");
	texture_ = SDL_CreateTextureFromSurface(renderer_, surface);
	SDL_FreeSurface(surface);
	playerPosition.x = 10.0;
	playerPosition.y = 20.0;
	playerVelocity.x = 100.0;
	playerVelocity.y = 200.0;
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
	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecs_prev_frame_);
	if (timeToWait > 0 && timeToWait <= millisecs_prev_frame_)
	{
		SDL_Delay(timeToWait);
	}

	float deltaTime = (SDL_GetTicks() - millisecs_prev_frame_) / 1000.0f;

	millisecs_prev_frame_ = SDL_GetTicks();

	playerPosition.x += playerVelocity.x * deltaTime;
	playerPosition.y += playerVelocity.y * deltaTime;

	if (playerPosition.x < 0 || playerPosition.x > window_width_ - 32)
	{
		Logger::Fatal("Boing!");
		playerVelocity.x = -playerVelocity.x;
	}

	if (playerPosition.y < 0 || playerPosition.y > window_height_ - 32)
	{
		Logger::Err("Hit Side");
		playerVelocity.y = -playerVelocity.y;
	}
}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer_, 0x15, 0x15, 0x15, 0xFF);
	SDL_RenderClear(renderer_);

	SDL_Rect dest = { static_cast<int>(playerPosition.x), static_cast<int>(playerPosition.y), 32, 32 };
	SDL_RenderCopy(renderer_, texture_, nullptr, &dest);

	SDL_RenderPresent(renderer_);
}

void Game::Shutdown()
{
	Logger::Log("Shutting down");
	SDL_DestroyTexture(texture_);
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();

	Logger::ShutDown();
}