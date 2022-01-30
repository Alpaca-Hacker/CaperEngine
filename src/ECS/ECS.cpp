#include "ECS/ECS.h"

#include "Log/Logger.h"

int IComponent::next_id = 0;

Entity::Entity(const int id) : id_(id)
{ }

void Entity::Kill()
{
	registry_->KillEntity(*this);
}

void Entity::Tag(const std::string& tag)
{
	registry_->TagEntity(*this, tag);
	Logger::Log("Entity {} has now got tag {}", this->GetId(), tag);
}

bool Entity::HasTag(const std::string& tag) const
{
	return registry_->EntityHasTag(*this, tag);
}

void Entity::Group(const std::string& group) 
{
	registry_->GroupEntity(*this, group);
}

bool Entity::BelongsToGroup(const std::string& group) const
{
	return registry_->EntityBelongsToGroup(*this, group);
}

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
	int entity_id;
	if (free_ids_.empty())
	{
		entity_id = num_entities_++;
		if (entity_id >= entity_component_signatures_.size()) {
			entity_component_signatures_.resize(entity_id + 1);
		}
	}
	else
	{
		entity_id = free_ids_.front();
		free_ids_.pop_front();
	}

	Entity entity(entity_id);
	entity.registry_ = this;
	entities_to_be_added_.insert(entity);

	Logger::Log("Entity created with id {}", entity_id);

	return entity;
}

void Registry::KillEntity(Entity entity)
{
	entities_to_be_removed_.insert(entity);
}

void Registry::TagEntity(Entity entity, const std::string& tag)
{
	entity_per_tag.emplace(tag, entity);
	tag_per_entity.emplace(entity.GetId(), tag);
}

bool Registry::EntityHasTag(Entity entity, const std::string& tag) const
{
	if (tag_per_entity.find(entity.GetId()) == tag_per_entity.end())
	{
		return false;
	}
	auto entity_found = entity_per_tag.find(tag);
	if (entity_found == entity_per_tag.end())
	{
		return false;
	}
	return entity_found->second == entity; 
}

Entity Registry::GetEntityByTag(const std::string& tag)
{
	return entity_per_tag.at(tag);
}

void Registry::RemoveEntityTag(Entity entity)
{
	auto tagged_entity = tag_per_entity.find(entity.GetId());
	if (tagged_entity != tag_per_entity.end())
	{
		auto tag = tagged_entity->second;
		entity_per_tag.erase(tag);
		tag_per_entity.erase(tagged_entity);
	}
}

void Registry::GroupEntity(Entity entity, const std::string& group)
{
	entity_per_group.emplace(group, std::set <Entity>());
	entity_per_group[group].emplace(entity);
	group_per_entity.emplace(entity.GetId(), group);
}

bool Registry::EntityBelongsToGroup(Entity entity, const std::string& group) const
{
	auto group_entities = entity_per_group.at(group);
	return group_entities.find(entity.GetId()) != group_entities.end();
}

std::vector<Entity> Registry::GetEntityByGroup(const std::string& group)
{
	auto& set_of_entities = entity_per_group.at(group);
	return std::vector<Entity>(set_of_entities.begin(), set_of_entities.end());
}

void Registry::RemoveEntityGroup(Entity entity)
{
	auto grouped_entity = group_per_entity.find(entity.GetId());
	if (grouped_entity != group_per_entity.end())
	{
		auto group = entity_per_group.find(grouped_entity->second);
		if (group != entity_per_group.end())
		{
			auto entity_in_group = group->second.find(entity);
			if(entity_in_group != group->second.end())
			{
				group->second.erase(entity_in_group);
			}
		}
		group_per_entity.erase(grouped_entity);
	}
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

void Registry::RemoveEntityFromSystems(Entity entity)
{
	for (auto& system : systems)
	{
		system.second->RemoveEntityFromSystem(entity);
	}
}

void Registry::Update()
{
	for (auto entity : entities_to_be_added_)
	{
		AddEntityToSystems(entity);
	}
	entities_to_be_added_.clear();

	for (auto entity : entities_to_be_removed_)
	{
		RemoveEntityFromSystems(entity);
		entity_component_signatures_[entity.GetId()].reset();
		free_ids_.push_back(entity.GetId());

		RemoveEntityTag(entity);
		RemoveEntityGroup(entity);
	}
	entities_to_be_removed_.clear();
}