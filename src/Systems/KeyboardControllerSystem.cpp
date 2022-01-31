#include "Systems/KeyboardControllerSystem.h"

#include "Components/KeyboardControlledComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"


void KeyboardControllerSystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
{
	eventBus->SubscribeToEvent<KeypressEvent>(this, &KeyboardControllerSystem::OnKeyPressed);
	eventBus->SubscribeToEvent<KeyReleaseEvent>(this, &KeyboardControllerSystem::OnKeyReleased);
}

void KeyboardControllerSystem::OnKeyPressed(KeypressEvent& event)
{
	//Logger::Log("Key {} ({}) pressed!", event.symbol, SDL_GetKeyName(event.symbol));

	for (auto entity : event.registry_.view<SpriteComponent, RigidBodyComponent, KeyboardControlledComponent>())
	{

		auto [sprite, rigid_body, keyboard] = event.registry_.get<SpriteComponent, RigidBodyComponent, KeyboardControlledComponent>(entity);

		switch (event.symbol_)
		{
		case SDLK_UP:
			rigid_body.velocity = keyboard.up_velocity;
			sprite.src_rect.y = sprite.height * 0;
			if (event.registry_.any_of<ProjectileEmitterComponent>(entity))
			{
				auto& emitter = event.registry_.get<ProjectileEmitterComponent>(entity);
				emitter.projectile_velocity = glm::vec2(0, -300);
			}
			break;
		case SDLK_RIGHT:
			rigid_body.velocity = keyboard.right_velocity;
			sprite.src_rect.y = sprite.height * 1;
			if (event.registry_.any_of<ProjectileEmitterComponent>(entity))
			{
				auto& emitter = event.registry_.get<ProjectileEmitterComponent>(entity);
				emitter.projectile_velocity = glm::vec2(300, 0);
			}
			break;
		case SDLK_DOWN:
			rigid_body.velocity = keyboard.down_velocity;
			sprite.src_rect.y = sprite.height * 2;
			if (event.registry_.any_of<ProjectileEmitterComponent>(entity))
			{
				auto& emitter = event.registry_.get<ProjectileEmitterComponent>(entity);
				emitter.projectile_velocity = glm::vec2(0, 300);
			}
			break;
		case SDLK_LEFT:
			rigid_body.velocity = keyboard.left_velocity;
			sprite.src_rect.y = sprite.height * 3;
			if (event.registry_.any_of<ProjectileEmitterComponent>(entity))
			{
				auto& emitter = event.registry_.get<ProjectileEmitterComponent>(entity);
				emitter.projectile_velocity = glm::vec2(-300, 0);
			}
			break;
		case SDLK_SPACE:
			if (event.registry_.any_of<ProjectileEmitterComponent>(entity))
			{
				auto& emitter = event.registry_.get<ProjectileEmitterComponent>(entity);
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

	for (auto entity : event.registry_.view<SpriteComponent, RigidBodyComponent, KeyboardControlledComponent>())
	{
		auto& rigid_body = event.registry_.get<RigidBodyComponent>(entity);

		switch (event.symbol_)
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

