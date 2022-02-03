#pragma once

#include "Events/KeypressEvent.h"

class KeyboardControllerSystem  {
public:

    void SubscribeToEvents(std::unique_ptr<entt::dispatcher>& dispatcher);

    void OnKeyPressed(KeypressEvent& event);
    void OnKeyReleased(KeyReleaseEvent& event);

    static void Update() { }

};