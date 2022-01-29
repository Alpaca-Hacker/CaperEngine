#pragma once
#include "Components/DebugComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/TransformComponent.h"
#include "ECS/ECS.h"

class DebugSystem : public System
{
public:
	DebugSystem()
	{
		RequireComponent<DebugComponent>();
	}

	void Update() {
		for (auto entity : GetSystemEntities())
		{
			const auto transform = entity.GetComponent<TransformComponent>();
			const auto rigid_body = entity.GetComponent<RigidBodyComponent>();

			Logger::Log("Entity {} now at position [{}, {}] with velocity [{}, {}]", entity.GetId(), transform.position.x, transform.position.y, 
				rigid_body.velocity.x, rigid_body.velocity.y);

		}
	}
};
