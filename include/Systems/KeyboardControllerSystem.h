#pragma once
#include "ECS/ECS.h"
#include "Events/EventBus.h"
#include "Events/KeypressEvent.h"

class KeyboardControllerSystem : public System {
public:
    KeyboardControllerSystem();

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);

    void OnKeyPressed(KeypressEvent& event);

    static void Update() { }
};