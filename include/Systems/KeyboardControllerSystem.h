#pragma once

#include "Events/EventBus.h"
#include "Events/KeypressEvent.h"

class KeyboardControllerSystem  {
public:

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);

    void OnKeyPressed(KeypressEvent& event);
    void OnKeyReleased(KeyReleaseEvent& event);

    static void Update() { }
};