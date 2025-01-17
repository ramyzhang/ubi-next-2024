#include "stdafx.h"

#include "SEntityManager.h"

//static SEntityManager& sEntityManager = SEntityManager::Instance();

EntityID SEntityManager::AddEntity(const std::string& tag) {
	assert(m_total_entities < MAX_ENTITIES);

	EntityID new_id = static_cast<EntityID>(m_total_entities);
	m_entities_to_add.push_back(Entity(tag, ComponentMask())); // add components later...
	m_total_entities++;

	return new_id;
}

void SEntityManager::Init() {
	m_total_entities = 0;

	// create the component pools!!!
	m_component_pool_map.insert_or_assign(CMESH, new ComponentPool<CMesh>());
	m_component_pool_map.insert_or_assign(CTRANSFORM, new ComponentPool<CTransform>());
	m_component_pool_map.insert_or_assign(CCOLLIDER, new ComponentPool<CCollider>());
	m_component_pool_map.insert_or_assign(CUITEXT, new ComponentPool<CUIText>());
	m_component_pool_map.insert_or_assign(CRIGIDBODY, new ComponentPool<CRigidBody>());
	// TODO: add more here as we create more components
}

void SEntityManager::Update() {
	// first, erase all inactive entities from the tag map
	for (EntityID i = 0; i < MAX_ENTITIES; i++) {
		if (m_entities[i].m_isactive) continue;
		if (std::holds_alternative<Entity*>(m_entities[i].m_state)) continue;

		const EntityLiveState& e_data = std::get<EntityLiveState>(m_entities[i].m_state);
		m_entity_map[e_data.m_tag].erase(i);
		for (auto& [id, pool] : m_component_pool_map) {
			std::visit([&](auto& concrete_pool) {
				concrete_pool->DisableComponent(i);
				}, pool);
		} // erase in component pools too

		m_entities.Remove(i);

		m_total_entities--;
	}

	// then, add in all the newly created entities to both the pool and tag map
	for (Entity& e : m_entities_to_add) {
		const EntityID new_id = m_entities.Add(e);
		const EntityLiveState& e_data = std::get<EntityLiveState>(e.m_state);

		// add to new (or existing) tag in tag map
		if (m_entity_map.count(e_data.m_tag) > 0) {
			m_entity_map[e_data.m_tag].insert(new_id);
		}
		else {
			m_entity_map.insert({ e_data.m_tag, {new_id} });
		}

		m_total_entities++;
	}

	// clear to_add vector
	m_entities_to_add.clear();
}

void SEntityManager::ClearAllEntities() {
	// reset the component pools (just a bitmap reset)
	for (auto& [id, pool] : m_component_pool_map) {
		std::visit([](auto& concrete_pool) {
			concrete_pool->ResetComponentPool();
			}, pool);
	}

	FreeContainer(m_entity_map);
	FreeContainer(m_entities_to_add);
	m_entities.ClearPool(); // have to thread through all the free slots again
	m_total_entities = 0;
}

void SEntityManager::Shutdown() {
	for (auto& [id, pool] : m_component_pool_map) {
		std::visit([](auto& concrete_pool) {
			delete concrete_pool;
			}, pool);
	}

	FreeContainer(m_component_pool_map);
	FreeContainer(m_entity_map);
	FreeContainer(m_entities);
}

void SEntityManager::ChangeTag(const EntityID& e, const std::string& new_tag) {
	Entity& entity = m_entities[e];

	EntityLiveState& e_data = std::get<EntityLiveState>(entity.m_state);
	std::string temp = e_data.m_tag;

	// look for entity in the entity map
	if (m_entity_map[temp].count(e) > 0) m_entity_map[temp].erase(e);

	// change tag in main entity vector
	e_data.m_tag = new_tag;

	// add to new tag in tag map
	if (m_entity_map.count(new_tag) > 0) {
		m_entity_map[new_tag].insert(e);
	} else {
		m_entity_map.insert({ new_tag, {e} });
	}
}
