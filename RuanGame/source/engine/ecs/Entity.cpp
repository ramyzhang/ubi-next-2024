#include "stdafx.h"

#include "Entity.h"

void Entity::AssignComponent(ComponentID comp_id) {
	int component_id = comp_id;
	std::get<EntityLiveState>(m_state).m_cmask.set(component_id);
}

void Entity::RemoveComponent(ComponentID comp_id) {
	int component_id = comp_id;
	std::get<EntityLiveState>(m_state).m_cmask.reset(component_id);
}
