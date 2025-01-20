#pragma once

#include "game/Event.h"
#include "engine/ecs/Entity.h"

const int MAX_OBSERVERS = 32; 

// subscribe to a subject to watch for notifications.
class Observer {
public:
    virtual ~Observer() {}
    virtual void OnNotify(Event event, std::vector<EntityID> entities) = 0;
};