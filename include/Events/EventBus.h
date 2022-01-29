#pragma once
#include <list>
#include <map>
#include <memory>
#include <typeindex>

#include "Event.h"

class IEventCallback
{
public:
	virtual ~IEventCallback() = default;

	void Execute(Event& e) { Call(e); }
private:
	virtual void Call(Event& e) = 0;
};

template <typename TOwner, typename TEvent>
class EventCallback : public IEventCallback
{
private:
	typedef void (TOwner::*CallbackFunction)(TEvent&);

	TOwner* owner_instance_;
	CallbackFunction callback_function_;

	virtual void Call(Event& e) override
	{
		std::invoke(callback_function_, owner_instance_, static_cast<TEvent&>(e));
	}
public:
	EventCallback(TOwner* owner_instance, CallbackFunction callback_function)
	{
		this->owner_instance_ = owner_instance;
		this->callback_function_ = callback_function;
	}

	virtual ~EventCallback() override = default;
};

typedef std::list <std::unique_ptr <IEventCallback>> HandlerList;

class EventBus
{
public:
	EventBus() = default;
	~EventBus() = default;

	template <typename TEvent, typename TOwner>
	void SubscribeToEvent(TOwner* owner_instance, void (TOwner::*callback_function)(TEvent&))
	{
		if (!subscribers_[typeid(TEvent)].get())
		{
			subscribers_[typeid(TEvent)] = std::make_unique<HandlerList>();
		}
		auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(owner_instance, callback_function);
		subscribers_[typeid(TEvent)]->push_back(std::move(subscriber));
	}

	template <typename TEvent, typename ...TArgs>
	void EmitEvent(TArgs&& ...args)
	{
		auto handlers = subscribers_[typeid(TEvent)].get();

		if (handlers)
		{
			for (auto it = handlers->begin(); it != handlers->end(); ++it)
			{
				auto handler = it->get();
				TEvent event(std::forward<TArgs>(args)...);
				handler->Execute(event);
			}
		}
	}

	void Reset()
	{
		subscribers_.clear();
	}

private:
	std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers_;
};
