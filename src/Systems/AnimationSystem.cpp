#include "Systems/AnimationSystem.h"

#include "Components/AnimationComponent.h"
#include "Components/SpriteComponent.h"

AnimationSystem::AnimationSystem()
{
	RequriedComponent<SpriteComponent>();
	RequriedComponent<AnimationComponent>();
}

void AnimationSystem::Update()
{
	for (auto entity : GetSystemEntities())
	{
		auto& sprite = entity.GetComponent<SpriteComponent>();
		auto& animation = entity.GetComponent<AnimationComponent>();

		animation.current_frame = ((SDL_GetTicks64() - animation.start_time) * animation.frame_speed_rate / 1000) % animation.num_frames;
		sprite.src_rect.x = animation.current_frame * sprite.width;
	}
}