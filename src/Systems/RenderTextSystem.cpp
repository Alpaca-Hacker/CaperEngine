#include "Systems/RenderTextSystem.h"

#include <entt/entt.hpp>

#include "AssetStore/AssetStore.h"
#include "Components/TextComponent.h"

void RenderTextSystem::Update(entt::registry& registry,  SDL_Renderer* renderer, std::unique_ptr<AssetStore>& asset_store, SDL_Rect& camera)
{
	for (auto entity : registry.view<TextComponent>())
	{
		const auto text_component = registry.get<TextComponent>(entity);

		SDL_Surface* surface = TTF_RenderText_Blended(
			asset_store->GetFont(text_component.asset_id), 
			text_component.text.c_str(), 
			text_component.colour);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		int text_width = 0;
		int text_height = 0;

		SDL_QueryTexture(texture, nullptr, nullptr, &text_width, &text_height);
		SDL_Rect dstRect{
			static_cast<int>(text_component.position.x - (text_component.is_fixed ? 0 : camera.x)),
			static_cast<int>(text_component.position.y - (text_component.is_fixed ? 0 : camera.y)),
			text_width,
			text_height
		};

		SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

		SDL_DestroyTexture(texture);
	}
}
