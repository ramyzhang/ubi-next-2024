#pragma once

#include <map>
#include <bitset>
#include "ECSConstants.h"
#include "Entity.h"

typedef std::bitset<MAX_ENTITIES> ComponentPoolMask;

template <typename T>
class ComponentPool {
public:
	ComponentPool() {
		m_cpool_mask.reset(); // ensure all bits are set to zero (not used yet)
	};
	
	bool CheckComponentExists(const EntityID& entity) {
		return m_cpool_mask.test(entity);
	};

	T& GetComponent(const EntityID& entity) const {
		return m_components[entity];
	};

	void CreateComponent(T& component, const EntityID& entity) {
		assert(m_cpool_mask.test(entity) == false);

		m_components[entity].SetData(component);
	};

	void DisableComponent(const EntityID& entity) {
		m_cpool_mask[entity].reset();
	};

private:
	T					m_components[MAX_ENTITIES];
	ComponentPoolMask	m_cpool_mask;
};