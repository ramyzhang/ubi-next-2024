#pragma once

#include <stdio.h>
#include <cassert>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <unordered_set>
#include <windows.h> // TODO: remove this (debuggin)
#include <variant>
#include "core/FreeContainer.h"
#include "core/System.h"
#include "EntityPool.h"
#include "ComponentPool.h"

// TODO: update whenever you make a new component type
using ComponentPoolVariant = std::variant<
    ComponentPool<CMesh>*,
    ComponentPool<CTransform>*,
    ComponentPool<CCollider>*,
    ComponentPool<CUIText>*
>;

typedef std::vector<Entity> EntityVector;
typedef std::unordered_set<EntityID> EntityIDSet;
typedef std::unordered_map<std::string, EntityIDSet> EntityMap;

typedef std::map<ComponentID, ComponentPoolVariant> ComponentPoolMap;

class SEntityManager : public System<SEntityManager> {
public:
    template<typename ...ComponentTypes>
    friend class EntityView;

    void Init(); // initialize component pools
    void Update(); // delayed update for iterator invalidation check, call first at new frame
    void ClearAllEntities();
    void Shutdown();

    EntityID AddEntity(const std::string& tag);

    const Entity& GetEntity(const EntityID& e) { 
        return m_entities.Get(e); 
    };
    EntityIDSet& GetEntities(const std::string& tag) { return m_entity_map[tag]; };

    size_t GetNumEntities() const { return m_total_entities; };

    void ChangeTag(const EntityID& e, const std::string& new_tag); // use this sparingly

    template <typename T>
    void AddComponent(const EntityID& e, T& c_data);
    template <typename T>
    void RemoveComponent(const EntityID& e);
    template <typename T>
    T* GetComponent(const EntityID& e);

private:
    EntityPool           m_entities;
    EntityMap            m_entity_map;
    EntityVector         m_entities_to_add;

    ComponentPoolMap     m_component_pool_map;

    size_t               m_total_entities;
};

template <typename T>
void SEntityManager::AddComponent(const EntityID& e, T& c_data) {
    assert(m_entities.Get(e).m_isactive);
    assert(m_component_pool_map.count(GetComponentID<T>()) > 0); // this component map should alr be created
    assert(!m_entities[e].CheckComponent(GetComponentID<T>()));

    ComponentPool<T>* component_pool = std::get<ComponentPool<T>*>(m_component_pool_map[GetComponentID<T>()]);
    assert(!component_pool->CheckComponentExists(e)); // check that component doesn't already exist

    // assign in the entity's bitmask
    EntityLiveState& e_data = std::get<EntityLiveState>(m_entities[e].m_state);
    e_data.m_cmask.set(GetComponentID<T>());

    // activate in the component pool
    component_pool->CreateComponent(c_data, e);
}

template <typename T>
void SEntityManager::RemoveComponent(const EntityID& e) {
    assert(m_entities[e].m_isactive);
    assert(m_component_pool_map.count(GetComponentID<T>()) > 0); // this component map should alr be created

    // remove from the entity's bitmask
    EntityLiveState& e_data = std::get<EntityLiveState>(m_entities[e].m_state);
    e_data.m_cmask.reset(GetComponentID<T>());

    // deactivate in the component pool
    ComponentPool<T>* component_pool = std::get<ComponentPool<T>*>(m_component_pool_map[GetComponentID<T>()]);
    component_pool->DisableComponent(e);
}

template<typename T>
T* SEntityManager::GetComponent(const EntityID& e) {
    assert(m_entities[e].m_isactive);
    assert(m_component_pool_map.count(GetComponentID<T>()) > 0); // this component map should alr be created
    assert(m_entities[e].CheckComponent(GetComponentID<T>()));

    ComponentPool<T>* component_pool = std::get<ComponentPool<T>*>(m_component_pool_map[GetComponentID<T>()]);
    assert(component_pool->CheckComponentExists(e));

    return component_pool->GetComponent(e);
}