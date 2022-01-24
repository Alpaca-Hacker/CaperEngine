#include "ECS/ECS.h"

#include "Log/Logger.h"

int IComponent::next_id = 0;

Entity::Entity(const int id) : id_(id)
{ }

void System::AddEntityToSystem(const Entity entity)
{
	entities_.push_back(entity);
}

void System::RemoveEntityFromSystem(const Entity entity)
{
	entities_.erase(std::remove_if(entities_.begin(), entities_.end(),
		[&entity](Entity other)
		{
			return entity == other;
		}),
		entities_.end());
}

Entity Registry::CreateEntity()
{
	int entity_id = num_entities_++;

	Entity entity(entity_id);
	entity.registry_ = this;
	entities_to_be_added_.insert(entity);

	if (entity_id >= entity_component_signatures_.size()) {
		entity_component_signatures_.resize(entity_id + 1);
	}

	Logger::Log("Entity created with id {}", entity_id);

	return entity;
}

void Registry::AddEntityToSystems(Entity entity)
{
	const auto entity_id = entity.GetId();

	const auto& entity_component_signature = entity_component_signatures_[entity_id];

	for (auto& system : systems)
	{
		const auto& system_component_signature = system.second->GetComponentSignature();

		if ((entity_component_signature & system_component_signature) == system_component_signature)
		{
			system.second->AddEntityToSystem(entity);
		}
	}
}

void Registry::Update()
{
	for (auto entity : entities_to_be_added_)
	{
		AddEntityToSystems(entity);
	}
	entities_to_be_added_.clear();
}