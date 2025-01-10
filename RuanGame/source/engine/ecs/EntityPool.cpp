#include "stdafx.h"

#include "EntityPool.h"

EntityPool::EntityPool() {
    for (int i = 0; i < MAX_ENTITIES - 1; i++) {
        m_entities[i].m_isactive = false;
        std::get<Entity*>(m_entities[i].m_state) = &m_entities[i + 1];
    }

    m_entities[MAX_ENTITIES - 1].m_isactive = false;
    std::get<Entity*>(m_entities[MAX_ENTITIES - 1].m_state) = nullptr;

    m_next_available = m_entities;
};

EntityPool::~EntityPool() {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        m_entities[i].m_isactive = false;
        m_entities[i].m_state = nullptr;
    }

    m_next_available = nullptr;
};

EntityID EntityPool::Add(const Entity& e) {
    EntityID new_index = static_cast<EntityID>(m_next_available - m_entities);
    m_next_available = std::get<Entity*>(m_entities[new_index].m_state);

    m_entities[new_index].m_state = e.m_state;
    m_entities[new_index].m_isactive = true;

    return new_index;
};

void EntityPool::Remove(const EntityID& e) {
    m_entities[e].m_state = m_next_available;
    m_entities[e].m_isactive = false;
    m_next_available = &m_entities[e];
};