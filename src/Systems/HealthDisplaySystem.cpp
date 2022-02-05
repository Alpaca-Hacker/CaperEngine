#include "Systems/HealthDisplaySystem.h"

#include <entt/entt.hpp>

#include "Components/HealthComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"

constexpr bool kShowPercentage = true;

void HealthDisplaySystem::Update(entt::registry& registry, SDL_Renderer* renderer, std::unique_ptr<AssetStore>& asset_store, SDL_Rect& camera)
{
	for (auto entity : registry.view<HealthComponent, SpriteComponent, TransformComponent>())
	{
		auto& health = registry.get<HealthComponent>(entity);
		auto& sprite = registry.get<SpriteComponent>(entity);
		auto& transform = registry.get<TransformComponent>(entity);

		float health_percent = static_cast<float>(health.health) / static_cast<float>(health.max_health);

		// Draw health bar
		SDL_Rect bar = {
			static_cast<int>(transform.position.x - 5 - camera.x),
			static_cast<int>(transform.position.y - 4 - camera.y),
			static_cast<int>((sprite.width + 5) * health_percent),
			5
		};
		SDL_Rect outline_bar = {
			static_cast<int>(transform.position.x - 5 - camera.x),
			static_cast<int>(transform.position.y - 4 - camera.y),
			sprite.width + 5,
			5
		};

		if (health_percent < 0.25) {
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
		}
		else if (health_percent >= 0.25 && health_percent <= 0.75)
		{
			SDL_SetRenderDrawColor(renderer, 0xFF, 0xD7, 0x00, 0x00);
		}
		else
		{
			SDL_SetRenderDrawColor(renderer, 0x32, 0xCD, 0x32, 0x00);
		}
		
		SDL_RenderFillRect(renderer, &bar);
		SDL_RenderDrawRect(renderer, &outline_bar);

		// Draw health percentage - although I don't think we need it..

		if constexpr (!kShowPercentage)
		{
			return;
		}

		std::string text = std::to_string(static_cast<int>(health_percent * 100)) + "%";

		SDL_Surface* surface = TTF_RenderText_Blended(
			asset_store->GetFont("charriot-font"),
			text.c_str(),
			SDL_Color{ 0xFF,0xFF,0xFF, 0xFF });

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		int text_width = 0;
		int text_height = 0;

		SDL_QueryTexture(texture, nullptr, nullptr, &text_width, &text_height);

		int x_pos = static_cast<int>(transform.position.x) + (sprite.width / 2) - (text_width / 2);

		SDL_Rect dstRect{
			x_pos -  camera.x,
			static_cast<int>(transform.position.y) - text_height - 3 -  camera.y,
			text_width,
			text_height
		};

		SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
		SDL_DestroyTexture(texture);
	}
}
