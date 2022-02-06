#include "Systems/RenderSystem.h"

#include <SDL.h>
#include <entt/entt.hpp>

#include "AssetStore/AssetStore.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"

struct EntityToRender
{
	TransformComponent transform_component;
	SpriteComponent sprite_component;
};

void RenderSystem::Update(const entt::registry& registry, SDL_Renderer* renderer, const std::unique_ptr<AssetStore>& asset_store, SDL_Rect camera)
{
	std::vector<EntityToRender> entities_to_render;

	for (auto entity : registry.view<TransformComponent, SpriteComponent>())
	{
		EntityToRender entity_to_render;
		auto [transform, sprite] = registry.get<TransformComponent, SpriteComponent>(entity);
		entity_to_render.transform_component = transform;
		entity_to_render.sprite_component = sprite;
		// Cull sprites outside the view

		bool is_entity_outside_camera_view = (
			entity_to_render.transform_component.position.x + (entity_to_render.transform_component.scale.x * entity_to_render.sprite_component.width) < camera.x ||
			entity_to_render.transform_component.position.x > camera.x + camera.w ||
			entity_to_render.transform_component.position.y + (entity_to_render.transform_component.scale.y * entity_to_render.sprite_component.height) < camera.y ||
			entity_to_render.transform_component.position.y > camera.y + camera.h
			);
		if (!is_entity_outside_camera_view || entity_to_render.sprite_component.is_fixed) {
			entities_to_render.emplace_back(entity_to_render);
		}
	}

	std::sort(entities_to_render.begin(),
		entities_to_render.end(),
		[](const EntityToRender& a, const EntityToRender& b)
		{
			return a.sprite_component.z_index < b.sprite_component.z_index;
		}
	);

	for (auto entity : entities_to_render)
	{
		const auto& transform = entity.transform_component;
		const auto& sprite = entity.sprite_component;

		SDL_Rect src_rect = sprite.src_rect;

		SDL_Rect dest_rect{
			static_cast<int>(transform.position.x - (sprite.is_fixed ? 0 : camera.x)),
			static_cast<int>(transform.position.y - (sprite.is_fixed ? 0 : camera.y)),
			static_cast<int>(static_cast<float>(sprite.width) * transform.scale.x),
			static_cast<int>(static_cast<float>(sprite.height) * transform.scale.y)
		};

		SDL_RenderCopyEx(
			renderer,
			asset_store->GetTexture(sprite.asset_id),
			&src_rect,
			&dest_rect,
			transform.rotation,
			nullptr,
			SDL_FLIP_NONE
		);
	}
}