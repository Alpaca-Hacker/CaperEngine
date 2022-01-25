#pragma once
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"

class BounceSystem : public System
{
public:
	BounceSystem()
	{
		RequriedComponent<TransformComponent>();
		RequriedComponent<RigidBodyComponent>();
	}

	void Update()
	{
		for (auto entity : GetSystemEntities())
		{
			const auto& transform = entity.GetComponent<TransformComponent>();
			auto& rigid_body = entity.GetComponent<RigidBodyComponent>();

			if (transform.position.x <= 0 || transform.position.x >= 780)
			{
				rigid_body.velocity.x = -rigid_body.velocity.x;
				rigid_body.velocity.x *= 1.25f;
			}

			if (transform.position.y <= 0 || transform.position.y >= 580)
			{
				rigid_body.velocity.y = -rigid_body.velocity.y;
				rigid_body.velocity.y *= 1.25f;
			}
		}
	}
};