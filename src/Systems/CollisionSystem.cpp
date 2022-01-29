#include "Systems/CollisionSystem.h"

#include "Components/BoxColliderComponent.h"
#include "Components/TransformComponent.h"
#include "Events/CollisionEvent.h"
#include "Events/EventBus.h"

CollisionSystem::CollisionSystem()
{
	RequireComponent<BoxColliderComponent>();
	RequireComponent<TransformComponent>();
}

void CollisionSystem::Update(std::unique_ptr<EventBus>& event_bus)
{
	auto entities = GetSystemEntities();
	for (auto i = entities.begin(); i != entities.end(); ++i)
	{
		Entity a = *i;
		const auto transform_a = i->GetComponent<TransformComponent>();
		auto& collider_a = i->GetComponent<BoxColliderComponent>();
		bool has_been_hit = false;

		for (auto j = i + 1; j != entities.end(); ++j)
		{
			Entity b = *j;
			const auto transform_b = j->GetComponent<TransformComponent>();
			auto& collider_b = j->GetComponent<BoxColliderComponent>();

			// TODO: Issue with scale needs fixing
			bool collisionHappened = CheckAABBCollision(
				(transform_a.position.x * transform_a.scale.x) + (collider_a.offset.x * transform_a.scale.x),
				transform_a.position.y * transform_a.scale.y + (collider_a.offset.y * transform_a.scale.y),
				collider_a.width,
				collider_a.height,
				transform_b.position.x * transform_b.scale.x + (collider_b.offset.x * transform_b.scale.x),
				transform_b.position.y * transform_b.scale.y + (collider_b.offset.y * transform_b.scale.y),
				collider_b.width,
				collider_b.height
			);
			if (collisionHappened)
			{
				Logger::Log("Hit happened between entities {} and {}", a.GetId(), b.GetId());

				event_bus->EmitEvent<CollisionEvent>(a, b);
			}
		}
		collider_a.is_hit = has_been_hit;
	}
}

bool CollisionSystem::CheckAABBCollision(double a_x, double a_y, double a_w, double a_h, double b_x, double b_y, double b_w, double b_h)
{
	return(
		a_x < b_x + b_w &&
		a_x + a_w > b_x &&
		a_y < b_y + b_h &&
		a_y + a_h > b_y
		);
}