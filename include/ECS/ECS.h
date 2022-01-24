#pragma once
#include <bitset>
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Pool.h"

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

	bool operator ==(const Entity& other) const { return id_ == other.id_; }
	bool operator !=(const Entity& other) const { return id_ != other.id_; }
	bool operator >(const Entity& other) const { return id_ > other.id_; }
	bool operator <(const Entity& other) const { return id_ < other.id_; }

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
	template<typename TComponent> void RequriedComponent();

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
	//void KillEntity(Entity entity);

	template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	template <typename TComponent> void RemoveComponent(Entity entity);
	template <typename TComponent> bool HasComponent(Entity entity);

	template <typename TSystem, typename ...TArgs> void AddSystem(Entity entity, TArgs&& ...args);
	template <typename TSystem> void RemoveSystem(Entity entity);
	template <typename TSystem> bool HasSystem(Entity entity);
	template <typename TSystem> TSystem& GetSystem() const;

	void AddEntityToSystems(Entity entity);
	//void RemoveSystem(Entity entity);

private:
	int num_entities_ = 0;
	std::set<Entity> entities_to_be_added_;
	std::set<Entity> entities_to_be_removed_;

	std::vector<IPool*> component_pools_;

	std::vector<Signature> entity_component_signatures_;
	std::unordered_map<std::type_index, System*> systems;
};

template <typename TComponent>
void System::RequriedComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	component_signature_.set(componentId);
}

template <typename TComponent, typename ... TArgs>
void Registry::AddComponent(Entity entity, TArgs&&... args)
{
	const auto component_id = Component<TComponent>::GetId();
	const auto entity_id = entity.GetId();

	if (component_id >= component_pools_.size)
	{
		component_pools_.resize(component_id + 1, nullptr);
	}

	if (!component_pools_[component_id])
	{
		Pool<TComponent>* new_component_pool = new Pool<TComponent>();
		component_pools_[component_id] = new_component_pool;
	}

	Pool<TComponent>* component_pool = component_pools_[component_id];

	if (entity_id >= component_pool->GetSize())
	{
		component_pool->Resize(num_entities_);
	}

	TComponent new_component(std::forward<TArgs>(args)...);

	component_pool->Set(entity_id, new_component);
	entity_component_signatures_[entity_id].set(component_id);
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

template <typename TSystem, typename ... TArgs>
void Registry::AddSystem(Entity entity, TArgs&&... args)
{
	TSystem* new_system(new TSystem(std::forward<TArgs>(args)...));
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