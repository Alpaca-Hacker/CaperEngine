#pragma once
#include "Components/RigidBodyComponent.h"
#include "Components/TransformComponent.h"

class MovementSystem 
{
public:
	MovementSystem() = default;


	void Update(entt::registry& registry, double delta_time)
	{
		const auto& cregistry = registry;
		for (auto entity : registry.view<TransformComponent, RigidBodyComponent>())
		{
			auto [transform, rigid_body] = registry.get<TransformComponent, RigidBodyComponent>(entity);

			auto distance = glm::vec2(0);
			if (registry.any_of<SpriteComponent>(entity))
			{
				const auto &sprite = cregistry.get<SpriteComponent>(entity);
				distance.x = sprite.width * transform.scale.x;
				distance.y = sprite.height * transform.scale.y;
			}
			
			transform.position.x = glm::clamp<float>(transform.position.x + (rigid_body.velocity.x * delta_time), 0, Engine::map_width_ - distance.x);
			
			transform.position.y = glm::clamp<float>(transform.position.y + (rigid_body.velocity.y * delta_time), 0, Engine::map_height_ - distance.y);

			//Logger::Log("Entity {} now at position [{}, {}]", entity.GetId(), transform.position.x, transform.position.y);
		}
	}
};
