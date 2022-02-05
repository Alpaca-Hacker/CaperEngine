#include "Systems/CollisionSystem.h"

#include <entt/entt.hpp>

#include "Components/BoxColliderComponent.h"
#include "Components/TransformComponent.h"
#include "Events/CollisionEvent.h"
#include "Log/Logger.h"


void CollisionSystem::Update(entt::registry& registry, std::unique_ptr<entt::dispatcher>& dispatcher)
{
	auto entities = registry.view<TransformComponent, BoxColliderComponent>();
	for (auto i = entities.begin(); i != entities.end(); ++i)
	{
		entt::entity a = *i;
		auto [transform_a, collider_a] = registry.get<TransformComponent, BoxColliderComponent>(a);
		bool has_been_hit = false;

		for (auto j = i; j != entities.end(); ++j)
		{
			if (i == j)
			{
				continue;
			}
		
			entt::entity b = *j;
			auto [transform_b, collider_b] = registry.get<TransformComponent, BoxColliderComponent>(b);

			// TODO: Issue with scale needs fixing
			bool collisionHappened = CheckAABBCollision(
				transform_a.position.x + (collider_a.offset.x * transform_a.scale.x),
				transform_a.position.y + (collider_a.offset.y * transform_a.scale.y),
				collider_a.width,
				collider_a.height,
				transform_b.position.x + (collider_b.offset.x * transform_b.scale.x),
				transform_b.position.y + (collider_b.offset.y * transform_b.scale.y),
				collider_b.width,
				collider_b.height
			);
			if (collisionHappened)
			{
				//Logger::Log("Hit happened between entities {} and {}", a, b);

 				dispatcher->enqueue<CollisionEvent>(a, b, &registry);
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