#include "stdafx.h"

#include "EntityPool.h"

EntityPool::EntityPool() {
    for (int i = 0; i < MAX_ENTITIES - 1; i++) {
        m_entities[i].m_isactive = false;
        m_entities[i].SetNext(&m_entities[i + 1]);
    }

    m_entities[MAX_ENTITIES - 1].SetNext(nullptr);
 
    m_next_available = m_entities;
};

EntityPool::~EntityPool() {
    m_next_available = nullptr;
};

EntityID EntityPool::Add(const Entity& e) {
    EntityID new_index = static_cast<EntityID>(m_next_available - m_entities);
    m_next_available = m_entities[new_index].GetNext();

    m_entities[new_index].SetData(std::get<EntityLiveState>(e.m_state));

    return new_index;
};

void EntityPool::Remove(const EntityID& e) {
    m_entities[e].SetNext(m_next_available);
    m_next_available = &m_entities[e];
}

void EntityPool::ClearPool() {
    for (int i = 0; i < MAX_ENTITIES - 1; i++) {
        m_entities[i].m_isactive = false;
        m_entities[i].SetNext(&m_entities[i + 1]);
    }

    m_entities[MAX_ENTITIES - 1].SetNext(nullptr);

    m_next_available = m_entities;
}
;