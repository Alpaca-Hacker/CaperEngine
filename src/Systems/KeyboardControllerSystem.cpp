#include "Systems/KeyboardControllerSystem.h"

#include "Components/KeyboardControlledComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Log/Logger.h"

// Not convinced that this is the best way, should try an input class that picks up events and this then processes them..

void KeyboardControllerSystem::SubscribeToEvents(std::unique_ptr<entt::dispatcher>& dispatcher)
{
	dispatcher->sink<KeypressEvent>().connect<&KeyboardControllerSystem::OnKeyPressed>(this);
	dispatcher->sink<KeyReleaseEvent>().connect<&KeyboardControllerSystem::OnKeyReleased>(this);
}

void KeyboardControllerSystem::OnKeyPressed(KeypressEvent& event)
{
	Logger::Log("Key {} ({}) pressed!", event.symbol, SDL_GetKeyName(event.symbol));

	for (auto entity : event.registry->view<SpriteComponent, RigidBodyComponent, KeyboardControlledComponent>())
	{

		auto [sprite, rigid_body, keyboard] = event.registry->get<SpriteComponent, RigidBodyComponent, KeyboardControlledComponent>(entity);

		switch (event.symbol)
		{
		case SDLK_UP:
			rigid_body.velocity = keyboard.up_velocity;
			sprite.src_rect.y = sprite.height * 0;
			if (event.registry->any_of<ProjectileEmitterComponent>(entity))
			{
				auto& emitter = event.registry->get<ProjectileEmitterComponent>(entity);
				emitter.projectile_velocity = glm::vec2(0, -300);
			}
			break;
		case SDLK_RIGHT:
			rigid_body.velocity = keyboard.right_velocity;
			sprite.src_rect.y = sprite.height * 1;
			if (event.registry->any_of<ProjectileEmitterComponent>(entity))
			{
				auto& emitter = event.registry->get<ProjectileEmitterComponent>(entity);
				emitter.projectile_velocity = glm::vec2(300, 0);
			}
			break;
		case SDLK_DOWN:
			rigid_body.velocity = keyboard.down_velocity;
			sprite.src_rect.y = sprite.height * 2;
			if (event.registry->any_of<ProjectileEmitterComponent>(entity))
			{
				auto& emitter = event.registry->get<ProjectileEmitterComponent>(entity);
				emitter.projectile_velocity = glm::vec2(0, 300);
			}
			break;
		case SDLK_LEFT:
			rigid_body.velocity = keyboard.left_velocity;
			sprite.src_rect.y = sprite.height * 3;
			if (event.registry->any_of<ProjectileEmitterComponent>(entity))
			{
				auto& emitter = event.registry->get<ProjectileEmitterComponent>(entity);
				emitter.projectile_velocity = glm::vec2(-300, 0);
			}
			break;
		case SDLK_SPACE:
			if (event.registry->any_of<ProjectileEmitterComponent>(entity))
			{
				auto& emitter = event.registry->get<ProjectileEmitterComponent>(entity);
				emitter.last_emission_time = -1000000;
			}
		default:
			break;
		}
	}
}

void KeyboardControllerSystem::OnKeyReleased(KeyReleaseEvent& event)
{
	//Logger::Log("Key {} ({}) released!", event.symbol, SDL_GetKeyName(event.symbol));

	for (auto entity : event.registry->view<SpriteComponent, RigidBodyComponent, KeyboardControlledComponent>())
	{
		auto& rigid_body = event.registry->get<RigidBodyComponent>(entity);

		switch (event.symbol)
		{
		case SDLK_UP:
			rigid_body.velocity.y = 0;
			break;
		case SDLK_RIGHT:
			rigid_body.velocity.x = 0;
			break;
		case SDLK_DOWN:
			rigid_body.velocity.y = 0;
			break;
		case SDLK_LEFT:
			rigid_body.velocity.x = 0;
			break;
		default:
			break;
		}
	}
}

