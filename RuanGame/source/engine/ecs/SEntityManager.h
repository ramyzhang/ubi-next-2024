#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <unordered_set>
#include <windows.h> // TODO: remove this (debuggin)
#include <variant>
#include "core/FreeContainer.h"
#include "core/System.h"
#include "game/components/ComponentIncludes.h"
#include "Component.h"
#include "ComponentPool.h"
#include "EntityPool.h"

using ComponentPoolVariant = std::variant<ComponentPool<CMesh>, ComponentPool<CTransform>>;

typedef std::vector<Entity> EntityVector;
typedef std::unordered_set<EntityID> EntityIDSet;
typedef std::unordered_map<std::string, EntityIDSet> EntityMap;

typedef std::map<ComponentID, ComponentPoolVariant> ComponentPoolMap;

class SEntityManager : public System<SEntityManager> {
public:
    void Init(); // initialize component pools
    void Update(); // delayed update for iterator invalidation check, call first at new frame
    void ClearAllEntities();
    void Shutdown();

    EntityID AddEntity(const std::string& tag);

    const Entity& GetEntity(const EntityID& e) { return m_entities.Get(e); };
    const Entity* GetEntities() { return m_entities.GetAll(); };
    EntityIDSet& GetEntities(const std::string& tag) { return m_entity_map[tag]; };

    size_t GetNumEntities() const { return m_total_entities; };

    void ChangeTag(const EntityID& e, const std::string& new_tag); // use this sparingly

    template <typename T>
    void AddComponent(const EntityID& e, const ComponentID& c, T& c_data);
    template <typename T>
    void RemoveComponent(const EntityID& e, const ComponentID& c);

private:
    EntityPool           m_entities;
    EntityMap            m_entity_map;
    EntityVector                m_entities_to_add;

    ComponentPoolMap     m_component_pool_map;

    size_t                      m_total_entities;
};

