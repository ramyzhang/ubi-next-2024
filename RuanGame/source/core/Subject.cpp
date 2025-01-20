#include "stdafx.h"

#include "Subject.h"

void Subject::AddObserver(Observer* observer) {
    if (m_num_observers >= MAX_OBSERVERS) return;

    m_observers.insert(observer);
    m_num_observers++;
}

void Subject::RemoveObserver(Observer* observer) {
    if (m_num_observers <= 0) return;

    // Iterate through the list to find the observer and remove it
    int index = 0;
    bool found = m_observers.erase(observer);

    if (!found) {
        OutputDebugStringA("Couldn't find observer.");
    }
    else {
        m_num_observers--;
    }
}

void Subject::Notify(Event event, std::vector<EntityID> entities) {
    for (auto& o : m_observers) {
        o->OnNotify(event, entities);
    }
}
