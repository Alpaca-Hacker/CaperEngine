#include "Engine/Engine.h"

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>

#include "Components/RigidBodyComponent.h"
#include "Components/TransformComponent.h"
#include "Log/Logger.h"

Engine::Engine()
{
	registry_ = std::make_unique <Registry>();
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

void Engine::Setup()
{
	Entity tank = registry_->CreateEntity();
	//Entity truck = registry_->CreateEntity();

	tank.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(10.0, 20.0));
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

void Engine::Update()
{
	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecs_prev_frame_);
	if (timeToWait > 0 && timeToWait <= millisecs_prev_frame_)
	{
		SDL_Delay(timeToWait);
	}

	double deltaTime = (SDL_GetTicks() - millisecs_prev_frame_) / 1000.0f;

	millisecs_prev_frame_ = SDL_GetTicks();
}

void Engine::Render()
{
	SDL_SetRenderDrawColor(renderer_, 0x15, 0x15, 0x15, 0xFF);
	SDL_RenderClear(renderer_);

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