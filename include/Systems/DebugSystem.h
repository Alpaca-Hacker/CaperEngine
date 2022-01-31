#pragma once
#include "Components/DebugComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/TransformComponent.h"

class DebugSystem 
{
public:

	void Update(entt::registry& registry) {
		for (auto entity : registry.view<DebugComponent>())
		{
			auto [transform, rigid_body] = registry.get<TransformComponent, RigidBodyComponent>(entity);

			Logger::Log("Entity {} now at position [{}, {}] with velocity [{}, {}]", entity, transform.position.x, transform.position.y, 
				rigid_body.velocity.x, rigid_body.velocity.y);

		}
	}
};
