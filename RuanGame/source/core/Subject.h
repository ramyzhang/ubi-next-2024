#pragma once

#include <unordered_set>
#include "Observer.h"
#include "engine/ecs/Entity.h"
#include "game/Event.h"

// send notifications to observers
class Subject {
public:
    void AddObserver(Observer* observer);
    void RemoveObserver(Observer* observer);

private:
    std::unordered_set<Observer*>   m_observers;
    int                             m_num_observers;

protected:
    void Notify(Event event, std::vector<EntityID> entities);
};