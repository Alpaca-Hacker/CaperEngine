#include "Systems/AnimationSystem.h"

#include <entt/entt.hpp>

#include "Components/AnimationComponent.h"
#include "Components/SpriteComponent.h"

void AnimationSystem::Update(entt::registry& registry)
{
	for (auto entity : registry.view<SpriteComponent, AnimationComponent>())
	{
		auto [sprite, animation] = registry.get<SpriteComponent, AnimationComponent>(entity);

		animation.current_frame = ((SDL_GetTicks64() - animation.start_time) * animation.frame_speed_rate / 1000) % animation.num_frames;
		sprite.src_rect.x = animation.current_frame * sprite.width;
	}
}