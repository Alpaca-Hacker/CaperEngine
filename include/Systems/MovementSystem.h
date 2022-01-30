#pragma once
#include "Components/RigidBodyComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"

class MovementSystem : public System
{
public:
	MovementSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void Update(double delta_time)
	{
		for (auto entity : GetSystemEntities())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto& rigid_body = entity.GetComponent<RigidBodyComponent>();
			auto distance = glm::vec2(0);
			if (entity.HasComponent<SpriteComponent>())
			{
				const auto sprite = entity.GetComponent<SpriteComponent>();
				distance.x = sprite.width * transform.scale.x;
				distance.y = sprite.height * transform.scale.y;
			}
			
			transform.position.x = glm::clamp<float>(transform.position.x + (rigid_body.velocity.x * delta_time), 0, Engine::map_width_ - distance.x);
			
			transform.position.y = glm::clamp<float>(transform.position.y + (rigid_body.velocity.y * delta_time), 0, Engine::map_height_ - distance.y);

			//Logger::Log("Entity {} now at position [{}, {}]", entity.GetId(), transform.position.x, transform.position.y);
		}
	}
};
