#pragma once

#include <cassert>
#include "ECSConstants.h"
#include "Entity.h"

// All entities are the same size (components are stored elsewhere),
// so let's store them in a pool!
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

