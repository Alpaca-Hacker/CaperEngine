#pragma once
struct HealthComponent
{
	int health;
	int max_health;

	HealthComponent(int health = 0) : health(health), max_health(health)
	{}
};