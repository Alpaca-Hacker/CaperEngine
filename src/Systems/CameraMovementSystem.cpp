#include "Systems/CameraMovementSystem.h"

#include "Components/CameraFollowConponent.h"
#include "Components/TransformComponent.h"
#include "Engine/Engine.h"
#include "glm/common.hpp"

CameraMovementSystem::CameraMovementSystem()
{
	RequireComponent<CameraFollowComponent>();
	RequireComponent<TransformComponent>();
}

void CameraMovementSystem::Update(SDL_Rect& camera)
{
	for (auto entity : GetSystemEntities())
	{
		auto& transform = entity.GetComponent<TransformComponent>();

			camera.x = glm::clamp<int>( transform.position.x - (camera.w / 2), 0, Engine::map_width_ - camera.w);


			camera.y = glm::clamp<int>(transform.position.y - (camera.h / 2), 0, Engine::map_height_ - camera.h);


		//Logger::Log("Camera now at position [{}, {}]", transform.position.x, transform.position.y);
	}
}

