#include "Systems/CameraMovementSystem.h"

#include "Components/CameraFollowConponent.h"
#include "Components/TransformComponent.h"
#include "Engine/Engine.h"
#include "glm/common.hpp"


void CameraMovementSystem::Update(entt::registry& registry, SDL_Rect& camera)
{
	for (auto entity : registry.view<TransformComponent, CameraFollowComponent>())
	{
		auto& transform = registry.get<TransformComponent>(entity);

		camera.x = glm::clamp<int>(transform.position.x - (camera.w / 2), 0, Engine::map_width_ - camera.w);
		camera.y = glm::clamp<int>(transform.position.y - (camera.h / 2), 0, Engine::map_height_ - camera.h);


		//Logger::Log("Camera now at position [{}, {}]", transform.position.x, transform.position.y);
	}
}

