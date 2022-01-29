#include "Systems/DamageSystem.h"

#include "Components/BoxColliderComponent.h"
#include "Events/CollisionEvent.h"
#include "Events/EventBus.h"

DamageSystem::DamageSystem()
{
	RequireComponent<BoxColliderComponent>();
}

void DamageSystem::OnCollision(CollisionEvent& event)
{
	//Logger::Log("Boom! Entities {} and {} crashed!", event.a.GetId(), event.b.GetId());
	//event.b.Kill();
}

void DamageSystem::SubscribeToEvents(std::unique_ptr<EventBus>& event_bus)
{
	event_bus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
}