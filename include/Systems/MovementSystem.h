#pragma once
#include "Components/RigidBodyComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"

class MovementSystem : public System
{
public:
	MovementSystem()
	{
		RequriedComponent<TransformComponent>();
		RequriedComponent<RigidBodyComponent>();
	}

	void Update(double delta_time)
	{
		for (auto entity : GetSystemEntities())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto& rigid_body = entity.GetComponent<RigidBodyComponent>();

			transform.position.x += rigid_body.velocity.x * delta_time;
			transform.position.y += rigid_body.velocity.y * delta_time;

			//Logger::Log("Entity {} now at position [{}, {}]", entity.GetId(), transform.position.x, transform.position.y);
		}
	}
};
