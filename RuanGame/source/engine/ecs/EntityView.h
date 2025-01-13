#pragma once

#include "SEntityManager.h"

// Use this custom iterator to iterate through currently active entities
template<typename ...ComponentTypes>
class EntityView {
public:
	EntityView(SEntityManager& em) : m_em(&em) {
		if (sizeof ...(ComponentTypes) == 0) {
			m_all = true; // if we just want all of the active entities
		}
		else {
			// unpack template params into list
			ComponentID c_ids[] = { GetComponentID<ComponentTypes>() ... };
			// select only the components we're looking for
			for (int i = 0; i < (sizeof ...(ComponentTypes)); i++) {
				m_cmask.set(c_ids[i]);
			}
		}
	};

	struct Iterator {
		Iterator(EntityID _id, SEntityManager* _em, ComponentMask _cmask, bool _all) :
			index(_id),
			em(_em),
			cmask(_cmask),
			all(_all) {};
		
		EntityID operator* () const {
			return index;
		};

		bool operator == (const Iterator& rhs) const {
			return index == rhs.index || index == MAX_ENTITIES;
		};

		bool operator != (const Iterator& rhs) const {
			return index != rhs.index && index != MAX_ENTITIES;
		};

		bool ValidIndex() {
			return
				em->GetEntity(index).IsActive() &&
				(all || cmask == (cmask & em->GetEntity(index).GetComponentMask()));
		};

		Iterator& operator ++ () {
			do {
				index++;
			} while (index < MAX_ENTITIES && !ValidIndex());

			return *this;
		};

		EntityID			index;
		SEntityManager*		em;
		ComponentMask		cmask;
		bool				all { false };
	};

	const Iterator begin() const
	{
		int first_i = 0;
		while (first_i < MAX_ENTITIES &&
			(m_cmask != (m_cmask & m_em->GetEntity(first_i).GetComponentMask())))
		{
			first_i++;
		}
		return Iterator(first_i, m_em, m_cmask, m_all);
	};

	const Iterator end() const
	{
		return Iterator(MAX_ENTITIES - 1, m_em, m_cmask, m_all);
	};

private:
	SEntityManager*		m_em;
	ComponentMask		m_cmask;
	bool				m_all { false };
};
