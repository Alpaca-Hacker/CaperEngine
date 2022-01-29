#pragma once
#include "ECS/ECS.h"

class ProjectileLifecycleSystem: public System
{
public:
	ProjectileLifecycleSystem();

	void Update();
};
