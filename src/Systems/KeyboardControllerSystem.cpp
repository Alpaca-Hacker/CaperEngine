#include "Systems/KeyboardControllerSystem.h"

#include "Components/KeyboardControlledComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"

KeyboardControllerSystem::KeyboardControllerSystem()
{
	RequireComponent<KeyboardControlledComponent>();
	RequireComponent<SpriteComponent>();
	RequireComponent<RigidBodyComponent>();
}

void KeyboardControllerSystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
{
	eventBus->SubscribeToEvent<KeypressEvent>(this, &KeyboardControllerSystem::OnKeyPressed);
	eventBus->SubscribeToEvent<KeyReleaseEvent>(this, &KeyboardControllerSystem::OnKeyReleased);
}

void KeyboardControllerSystem::OnKeyPressed(KeypressEvent& event)
{
	//Logger::Log("Key {} ({}) pressed!", event.symbol, SDL_GetKeyName(event.symbol));

	for (auto entity : GetSystemEntities())
	{
		auto& sprite = entity.GetComponent<SpriteComponent>();
		auto& rigid_body = entity.GetComponent<RigidBodyComponent>();
		const auto &keyboard = entity.GetComponent<KeyboardControlledComponent>();

		switch (event.symbol)
		{
		case SDLK_UP:
			rigid_body.velocity = keyboard.up_velocity;
			sprite.src_rect.y = sprite.height * 0;
			break;
		case SDLK_RIGHT:
			rigid_body.velocity = keyboard.right_velocity;
			sprite.src_rect.y = sprite.height * 1;
			break;
		case SDLK_DOWN:
			rigid_body.velocity = keyboard.down_velocity;
			sprite.src_rect.y = sprite.height * 2;
			break;
		case SDLK_LEFT:
			rigid_body.velocity = keyboard.left_velocity;
			sprite.src_rect.y = sprite.height * 3;
			break;
		default:
			break;
		}
	}
}

void KeyboardControllerSystem::OnKeyReleased(KeyReleaseEvent& event)
{
	//Logger::Log("Key {} ({}) released!", event.symbol, SDL_GetKeyName(event.symbol));

	for (auto entity : GetSystemEntities())
	{
		auto& rigid_body = entity.GetComponent<RigidBodyComponent>();

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

