#include "stdafx.h"

#include "SEntityManager.h"
#include <cassert>

//static SEntityManager& sEntityManager = SEntityManager::Instance();

EntityID SEntityManager::AddEntity(const std::string& tag) {
	assert(m_total_entities >= MAX_ENTITIES);

	EntityID new_id = static_cast<EntityID>(m_total_entities);
	m_entities_to_add.push_back(Entity(tag, ComponentMask())); // add components later
	m_total_entities++;

	return new_id;
}

void SEntityManager::Init() {
	m_entities = EntityPool();
	m_component_pool_map.insert_or_assign(CMESH, ComponentPool<CMesh>());
	m_component_pool_map.insert_or_assign(CTRANSFORM, ComponentPool<CTransform>());
	// TODO: add more here as we create more components
}

void SEntityManager::Update() {
	// first, erase all inactive entities from the tag map
	const Entity* entities = m_entities.GetAll();

	for (EntityID i = 0; i < MAX_ENTITIES; i++) {
		if (entities[i].m_isactive) continue;

		if (std::holds_alternative<Entity*>(entities[i].m_state)) continue;
		EntityLiveState e_state = std::get<EntityLiveState>(entities[i].m_state);
		m_entity_map[e_state.m_tag].erase(i);

		m_entities.Remove(i);

		m_total_entities--;
	}

	// then, add in all the newly created entities
	for (Entity& e : m_entities_to_add) {
		EntityLiveState& e_state = std::get<EntityLiveState>(e.m_state);
		e.m_isactive = true;

		const EntityID new_id = m_entities.Add(e);
		// add to new (or existing) tag in tag map
		if (m_entity_map.count(e_state.m_tag) > 0) {
			m_entity_map[e_state.m_tag].insert(new_id);
		}
		else {
			m_entity_map.insert({ e_state.m_tag, {new_id} });
		}

		m_total_entities++;
	}

	// clear to_add vector
	m_entities_to_add.clear();
}

void SEntityManager::ClearAllEntities() {
	FreeContainer(m_entity_map);
	FreeContainer(m_entities);
}

void SEntityManager::Shutdown() {
	m_entities.~EntityPool();
}

void SEntityManager::ChangeTag(const EntityID& e, const std::string& new_tag) {
	Entity& entity = m_entities[e];
	EntityLiveState& e_state = std::get<EntityLiveState>(entity.m_state);
	std::string temp = e_state.m_tag;

	// look for entity in the entity map
	if (m_entity_map[temp].count(e) > 0) m_entity_map[temp].erase(e);

	// change tag in main entity vector
	e_state.m_tag = new_tag;

	// add to new tag in tag map
	if (m_entity_map.count(new_tag) > 0) {
		m_entity_map[new_tag].insert(e);
	} else {
		m_entity_map.insert({ new_tag, {e} });
	}
}

template <typename T>
void SEntityManager::AddComponent(const EntityID& e, const ComponentID& c, T& c_data) {
	assert(m_component_pool_map.count(c) > 0); // this component map should alr be created

	ComponentPool<T>& component_pool = std::get<ComponentPool<T>>(m_component_pool_map[c]);
	assert(!component_pool.CheckComponentExists(e)); // check that component doesn't already exist

	// assign in the entity's bitmask
	m_entities[e].AssignComponent(c);

	// activate in the component pool
	component_pool.CreateComponent(std::get<T>(c_data), e);
}

template <typename T>
void SEntityManager::RemoveComponent(const EntityID& e, const ComponentID& c) {
	assert(m_component_pool_map.count(c) > 0); // this component map should alr be created

	m_entities[e].RemoveComponent(c);

	ComponentPool<T>& component_pool = std::get<ComponentPool<T>>(m_component_pool_map[c]);
	component_pool.DisableComponent(e);
}
