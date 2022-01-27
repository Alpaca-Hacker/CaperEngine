#include "Systems/RenderSystem.h"

#include <SDL.h>

#include "AssetStore/AssetStore.h"

struct EntityToRender
{
	TransformComponent transform_component;
	SpriteComponent sprite_component;
};

void RenderSystem::Update(SDL_Renderer* renderer, const std::unique_ptr<AssetStore>& asset_store)
{
	std::vector<EntityToRender> entities_to_render;

	for (auto entity : GetSystemEntities())
	{
		EntityToRender entity_to_render;
		entity_to_render.transform_component = entity.GetComponent<TransformComponent>();
		entity_to_render.sprite_component = entity.GetComponent<SpriteComponent>();
		entities_to_render.emplace_back(entity_to_render);
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
			static_cast<int>(transform.position.x),
			static_cast<int>(transform.position.y),
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