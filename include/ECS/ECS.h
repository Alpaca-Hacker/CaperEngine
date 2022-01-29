#pragma once
#include <bitset>
#include <deque>
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Pool.h"
#include "Log/Logger.h"

const unsigned int MAX_COMPONENTS = 32;

typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent
{
protected:
	static int next_id;
};

//used to assign unique id to a component type
template <typename T>
class Component : public IComponent
{
public:
	// Returns the unique id of Component<TComponent>
	static int GetId()
	{
		static auto id = next_id++;
		return id;
	}
};

class Entity
{
public:
	Entity(const int id);

	[[nodiscard]] int GetId() const { return id_; }
	void Kill();
	template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
	template <typename TComponent> void RemoveComponent();
	template <typename TComponent> bool HasComponent() const;
	template <typename TComponent> TComponent& GetComponent() const;

	bool operator ==(const Entity& other) const { return id_ == other.id_; }
	bool operator !=(const Entity& other) const { return id_ != other.id_; }
	bool operator >(const Entity& other) const { return id_ > other.id_; }
	bool operator <(const Entity& other) const { return id_ < other.id_; }

	class Registry* registry_;

private:
	int id_;
};

class System
{
public:
	System() = default;
	virtual ~System() = default;

	void AddEntityToSystem(Entity entity);
	void RemoveEntityFromSystem(Entity entity);
	[[nodiscard]] std::vector<Entity> GetSystemEntities() const { return entities_; }
	[[nodiscard]] const Signature& GetComponentSignature() const { return component_signature_; }

	// Defines the component type that the entities must have
	template<typename TComponent> void RequireComponent();

private:
	Signature component_signature_;
	std::vector<Entity> entities_;
};

class Registry
{
public:
	Registry() = default;
	void Update();

	Entity CreateEntity();
	void KillEntity(Entity entity);

	template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	template <typename TComponent> void RemoveComponent(Entity entity);
	template <typename TComponent> bool HasComponent(Entity entity);
	template <typename TComponent> TComponent& GetComponent(Entity entity) const;

	template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
	template <typename TSystem> void RemoveSystem(Entity entity);
	template <typename TSystem> bool HasSystem(Entity entity);
	template <typename TSystem> TSystem& GetSystem() const;

	void AddEntityToSystems(Entity entity);
	void RemoveEntityFromSystems(Entity entity);

private:
	int num_entities_ = 0;
	std::set<Entity> entities_to_be_added_;
	std::set<Entity> entities_to_be_removed_;

	std::vector<std::shared_ptr<IPool>> component_pools_;

	std::vector<Signature> entity_component_signatures_;
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
	std::deque<int> free_ids_;
};

/*
 *		TEMPLATES
 */

template <typename TComponent>
void System::RequireComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	component_signature_.set(componentId);
}

template <typename TComponent, typename ... TArgs>
void Registry::AddComponent(Entity entity, TArgs&&... args)
{
	const auto component_id = Component<TComponent>::GetId();
	const auto entity_id = entity.GetId();

	if (component_id >= component_pools_.size())
	{
		component_pools_.resize(component_id + 1, nullptr);
	}

	if (!component_pools_[component_id])
	{
		std::shared_ptr<Pool<TComponent>> new_component_pool = std::make_shared<Pool<TComponent>>();
		component_pools_[component_id] = new_component_pool;
	}

	std::shared_ptr <Pool<TComponent>> component_pool = std::static_pointer_cast<Pool<TComponent>>(component_pools_[component_id]);

	if (entity_id >= component_pool->GetSize())
	{
		component_pool->Resize(num_entities_);
	}

	TComponent new_component(std::forward<TArgs>(args)...);

	component_pool->Set(entity_id, new_component);

	entity_component_signatures_[entity_id].set(component_id);

	Logger::Log("Component id {} was added to entity id {}", component_id, entity_id);
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
	const auto component_id = Component<TComponent>::GetId();
	const auto entity_id = entity.GetId();

	entity_component_signatures_[entity_id].set(component_id, false);
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity)
{
	const auto component_id = Component<TComponent>::GetId();
	const auto entity_id = entity.GetId();

	return entity_component_signatures_[entity_id].test(component_id);
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const
{
	const auto component_id = Component<TComponent>::GetId();
	const auto entity_id = entity.GetId();

	auto component_pool = std::static_pointer_cast<Pool<TComponent>>(component_pools_[component_id]);
	return component_pool->Get(entity_id);
}

template <typename TSystem, typename ... TArgs>
void Registry::AddSystem(TArgs&&... args)
{
	std::shared_ptr<TSystem> new_system = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), new_system));
}

template <typename TSystem>
void Registry::RemoveSystem(Entity entity)
{
	const auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem(Entity entity)
{
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const
{
	const auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename ... TArgs>
void Entity::AddComponent(TArgs&&... args)
{
	registry_->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent()
{
	registry_->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const
{
	return registry_->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const
{
	return registry_->GetComponent<TComponent>(*this);
}
