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
			
			transform.position.x = glm::clamp<float>(transform.position.x + (rigid_body.velocity.x * delta_time), 0, Engine::map_width_ - 64);
			
			transform.position.y = glm::clamp<float>(transform.position.y + (rigid_body.velocity.y * delta_time), 0, Engine::map_height_ - 64);

			//Logger::Log("Entity {} now at position [{}, {}]", entity.GetId(), transform.position.x, transform.position.y);
		}
	}
};
