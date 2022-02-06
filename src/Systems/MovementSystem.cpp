#include "Systems/MovementSystem.h"

#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/Tags.h"
#include "Components/TransformComponent.h"
#include "glm/common.hpp"

void MovementSystem::Update(entt::registry& registry, double delta_time)
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

void MovementSystem::OnCollision(CollisionEvent& event)
{

	if (!event.registry->valid(event.a) || !event.registry->valid(event.b))
	{
		return;
	}

	entt::entity a = event.a;
	entt::entity b = event.b;

	if (event.registry->any_of<Obstacle>(a) && event.registry->any_of<Enemy>(b)) {
		OnObstacleHitsEnemy(a, b, event.registry);
	}
	if (event.registry->any_of<Obstacle>(b) && event.registry->any_of<Enemy>(a)) {
		OnObstacleHitsEnemy(b, a, event.registry);
	}

}


void MovementSystem::OnObstacleHitsEnemy(entt::entity& obstacle, entt::entity& enemy, entt::registry* registry)
{
	auto [transform, rigid_body] = registry->get<TransformComponent, RigidBodyComponent>(enemy);

	transform.rotation += 180.0;
	if (transform.rotation >= 360)
	{
		transform.rotation -= 360;
	}

	rigid_body.velocity.x = -rigid_body.velocity.x;
	rigid_body.velocity.y = -rigid_body.velocity.y;
}


