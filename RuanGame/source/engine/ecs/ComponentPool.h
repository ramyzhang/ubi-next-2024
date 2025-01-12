#pragma once

#include <cassert>
#include <map>
#include <bitset>
#include "ECSConstants.h"
#include "Entity.h"

typedef std::bitset<MAX_ENTITIES> ComponentPoolMask;

template <typename T>
class ComponentPool {
public:
	ComponentPool() {
		m_components = new T[MAX_ENTITIES]; // dynamically allocate
		m_cpool_mask.reset(); // ensure all bits are set to zero (not used yet)
	};

	~ComponentPool() {
		delete[] m_components;
	}

	void ResetComponentPool() {
		m_cpool_mask.reset();
	};

	bool CheckComponentExists(const EntityID& entity) {
		return m_cpool_mask.test(entity);
	};

	T* GetComponent(const EntityID& entity) const {
		assert(m_cpool_mask.test(entity));
		return m_components + entity;
	};

	void CreateComponent(T& component, const EntityID& entity) {
		assert(!m_cpool_mask.test(entity));
		m_components[entity] = component;
		m_cpool_mask.set(entity);
	};

	void DisableComponent(const EntityID& entity) {
		m_cpool_mask.reset(entity);
	};
	
private:
	T*					m_components { nullptr };
	ComponentPoolMask	m_cpool_mask;
};