#pragma once

#include <stdio.h>
#include <math.h>
#include <string>
#include <bitset>
#include <variant>
#include "ECSConstants.h"
#include "Component.h"

typedef unsigned int EntityID;
typedef std::bitset<MAX_COMPONENTS> ComponentMask;

struct EntityLiveState {
    std::string     m_tag;
    ComponentMask   m_cmask;
}; // define struct for handling entity data when it's alive

class Entity {
public:
    // only allow managers and pools to access private variables
    friend class SEntityManager; 
    friend class EntityPool;

    std::string GetTag() const { return std::get<EntityLiveState>(m_state).m_tag; };
    bool IsActive() const { return m_isactive; };

    void Destroy() { m_isactive = false; };

private:
    // Constructor is private; only EntityManager can make new Entities
    Entity(const std::string& tag, const ComponentMask cmask)
        : m_state{ EntityLiveState{tag, cmask} } // Direct init of variant with LiveState
    {}

    Entity() : m_state(nullptr) {}; // Direct init of variant with "next" pointer

    void AssignComponent(ComponentID comp_id); // only for EntityManager to use
    void RemoveComponent(ComponentID comp_id); // only for EntityManager to use

    Entity*   GetNext() const { return std::get<Entity*>(m_state); };
    void      SetNext(Entity* next) { m_state = next; }; // warning: this sets the entity to inactive!
    void      SetData(const EntityLiveState& e_state) { m_state = e_state; };

    bool m_isactive = false;

    // entity pool-friendly union!
    // it either contains the entity's data or a pointer to the next available entity
    std::variant<EntityLiveState, Entity*> m_state = nullptr;
};