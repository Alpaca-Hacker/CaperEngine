#include <Systems/JanitorSystem.h>

#include "Components/Tags.h"

void JanitorSystem::Update(entt::registry& registry_)
{
	for (auto entity : registry_.view<Destroy>())
	{
		registry_.destroy(entity);
	}
}
