#pragma once

#include <memory>
#include "engine/EngineIncludes.h"

class SSpawner {
public:
	void InstantiateFloor(const EntityID& _eid, Vector3 _colour, Vector3 _position, Vector3 _scale);
	void InstantiateCube(const EntityID& eid, Vector3 _colour, Vector3 _position, Vector3 _scale, bool _is_static);
	void InstantiateBound(const EntityID& _eid, Vector3 _position, Vector3 _scale);
	void InstantiateBoid(const EntityID& _eid, Vector3 _position);
	void InstantiatePlayer(const EntityID& _eid);
	void InstantiatePathDodec(const EntityID& eid, Vector3 _colour, Vector3 _position);
	void InstantiateStar(const EntityID& _eid, Vector3 _position);
};