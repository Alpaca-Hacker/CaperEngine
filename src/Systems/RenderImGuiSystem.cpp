#include "Systems/RenderImGuiSystem.h"

#include <entt/entt.hpp>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "Components/BoxColliderComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/Tags.h"
#include "Components/TransformComponent.h"


void RenderImGuiSystem::Update(entt::registry& registry, SDL_Renderer* renderer, std::unique_ptr<AssetStore>& asset_store, SDL_Rect camera)
{

	static int pos[2] = {0, 0 };
	static float scale[2] = { 1.0, 1.0 };
	static float rotation = 0.0;
	static float vel[2] = { 0.0, 0.0 };
	static int health = 100;
	const char* assets[] = { "tank-blue", "tank-dark", "tank-red", "tank-green", "tank-sand"};
	static int item_current = 0;
	static float rate_of_fire = 5;
	static float projectile_duration = 10;
	static int speed_of_projectile = 100;
	static int projectile_damage = 10;

	const int x_pos = pos[0];
	const int y_pos = pos[1];
	const float scale_x = scale[0];
	const float scale_y = scale[1];
	const glm::vec2 velocity = glm::vec2(vel[0], vel [1]);


	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow();

	if (ImGui::Begin("Spawn enemies"))
	{
		ImGui::DragInt2("Position", pos);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Use ctrl to type");
		ImGui::DragFloat2("Scale", scale);
		ImGui::SliderFloat("Rotation", &rotation, -180.0, 180.0);
		ImGui::DragFloat2("Velocity", vel);
		ImGui::DragInt("Health", &health);

		ImGui::Combo("Texture", &item_current, assets, IM_ARRAYSIZE(assets));

		ImGui::Spacing();
		ImGui::Text("Projectile");
		ImGui::InputInt("Speed", &speed_of_projectile);
		ImGui::InputFloat("Delay Between Projectiles (s)", &rate_of_fire);
		ImGui::InputFloat("Duration (s)", &projectile_duration);
		ImGui::InputInt("Damage", &projectile_damage);

		if (ImGui::Button("Create new enemy"))
		{
			auto entity = registry.create();
			registry.emplace<Enemy>(entity);
			registry.emplace<TransformComponent>(entity, glm::vec2(x_pos, y_pos), glm::vec2(scale_x, scale_y), rotation);
			registry.emplace<RigidBodyComponent>(entity, velocity);
			registry.emplace<SpriteComponent>(entity, assets[item_current], 42, 46, 2);
			registry.emplace<BoxColliderComponent>(entity, 42, 46);
			registry.emplace<ProjectileEmitterComponent>(entity, speed_of_projectile, rate_of_fire*1000, projectile_duration*1000, projectile_damage, false);
			registry.emplace<HealthComponent>(entity,health);
		}
	}
	ImGui::End();


	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

	//Render example object

	SDL_Rect src_rect{ 0,0,42,46 };
	SDL_Rect dest_rect{
	(x_pos - camera.x),
	(y_pos - camera.y),
	static_cast<int>(42 * scale_x),
	static_cast<int>(46 * scale_y)
	};
	auto texture = asset_store->GetTexture(assets[item_current]);
	SDL_SetTextureAlphaMod(texture, 128);
	SDL_RenderCopyEx(
		renderer,
		texture,
		&src_rect,
		&dest_rect,
		rotation,
		nullptr,
		SDL_FLIP_NONE
	);
	SDL_SetTextureAlphaMod(texture, 255);
}
