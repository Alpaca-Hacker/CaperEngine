#pragma once
#include "ECS/ECS.h"

class ProjectileEmitterSystem : public System
{
public:
	ProjectileEmitterSystem();

	void Update(std::unique_ptr<Registry>& registry);
};
