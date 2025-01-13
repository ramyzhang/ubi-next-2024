#pragma once

#include <cassert>
#include "ECSConstants.h"
#include "Entity.h"

// Since all the entities are the same size (just an id and a bitmask),
// Let's use an object pool to store them!
// Based on the object pool design from the Game Programming Patterns book
class EntityPool {
public:
    // thread all the "available" slots together into a 
    // linked list (which is all of them right now)
    EntityPool();

    ~EntityPool();

    EntityID Add(const Entity& e);

    void Remove(const EntityID& e);

    Entity& operator [] (const EntityID& e) {
        assert(e < MAX_ENTITIES);
        return m_entities[e];
    }

    Entity& Get(const EntityID& e) {
        assert(e < MAX_ENTITIES);
        return m_entities[e];
    }

    const Entity* GetAll() const {
        return m_entities;
    }

private:
    Entity      m_entities[MAX_ENTITIES];
    Entity*     m_next_available;
};

