#pragma once
#include "ECS/ECS.h"
#include "Events/EventBus.h"

class CollisionSystem : public System
{
public:
	CollisionSystem();

	void Update(std::unique_ptr<EventBus>& event_bus);
	bool CheckAABBCollision(double a_x, double a_y, double a_w, double a_h,
		double b_x, double b_y, double b_w, double b_h);
};
