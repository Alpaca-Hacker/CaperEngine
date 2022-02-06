#include "Systems/KeyboardControllerSystem.h"

#include "Components/BoxColliderComponent.h"
#include "Components/KeyboardControlledComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/Tags.h"
#include "Components/TransformComponent.h"
#include "glm/detail/func_trigonometric.inl"
#include "Log/Logger.h"

// Not convinced that this is the best way, should try an input class that picks up events and this then processes them..

void KeyboardControllerSystem::SubscribeToEvents(std::unique_ptr<entt::dispatcher>& dispatcher)
{
	dispatcher->sink<KeypressEvent>().connect<&KeyboardControllerSystem::OnKeyPressed>(this);
	dispatcher->sink<KeyReleaseEvent>().connect<&KeyboardControllerSystem::OnKeyReleased>(this);
}

void KeyboardControllerSystem::OnKeyPressed(KeypressEvent& event)
{
	//Logger::Log("Key {} ({}) pressed!", event.symbol, SDL_GetKeyName(event.symbol));

	for (auto entity : event.registry->view<Player>()) //player is the only one keyboard controlled
	{
		static int direction = 0;
		static glm::vec2 velocity = { 0,-1 };
		static int last_fire_time = SDL_GetTicks64();


		auto [sprite, rigid_body, keyboard, transform, emitter] =
			event.registry->get<SpriteComponent, RigidBodyComponent, KeyboardControlledComponent, TransformComponent, ProjectileEmitterComponent>(entity);

		switch (event.symbol)
		{
		case SDLK_UP:
			rigid_body.velocity = keyboard.up_velocity;
			direction = 0;
			velocity = { 0,-1 };
			break;
		case SDLK_RIGHT:
			rigid_body.velocity = keyboard.right_velocity;
			direction = 1;
			velocity = { 1,0 };
			break;
		case SDLK_DOWN:
			rigid_body.velocity = keyboard.down_velocity;
			direction = 2;
			velocity = { 0,1 };
			break;
		case SDLK_LEFT:
			rigid_body.velocity = keyboard.left_velocity;
			direction = 3;
			velocity = { -1,0 };
			break;
		case SDLK_SPACE:
		{
			if (SDL_GetTicks64() - last_fire_time > emitter.repeat_frequency)
			{
				glm::vec2 position = transform.position;
				position.x += (transform.scale.x * sprite.width / 2);
				position.y += (transform.scale.y * sprite.height / 2);

				auto vel = glm::vec2(velocity.x * emitter.projectile_speed, velocity.y * emitter.projectile_speed);
				auto projectile = event.registry->create();
				event.registry->emplace<TransformComponent>(projectile, position);
				event.registry->emplace<RigidBodyComponent>(projectile, vel);
				event.registry->emplace<SpriteComponent>(projectile, "bullet-image", 4, 4, 4);
				event.registry->emplace<BoxColliderComponent>(projectile, 4, 4, glm::vec2(0, 0));
				event.registry->emplace<ProjectileComponent>(projectile, emitter.is_friendly, emitter.hit_percent_damage, emitter.projectile_duration);
				event.registry->emplace<Projectile>(projectile);

				last_fire_time = SDL_GetTicks64();
			}
		}
			break;
		default:
			break;
		}

		sprite.src_rect.y = sprite.height * direction;
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