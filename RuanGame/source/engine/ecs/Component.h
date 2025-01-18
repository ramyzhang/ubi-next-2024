#pragma once

#include "components/ComponentIncludes.h"

// TODO: update this when you make a new component
enum ComponentID {
	CTRANSFORM,
	CMESH,
	CCOLLIDER,
	CUITEXT,
	CRIGIDBODY,
	CBOID
};

template <typename T>
inline ComponentID GetComponentID() { return -1; };

template <>
inline ComponentID GetComponentID<CTransform>() { return CTRANSFORM; };

template <>
inline ComponentID GetComponentID<CMesh>() { return CMESH; };

template <>
inline ComponentID GetComponentID<CCollider>() { return CCOLLIDER; };

template <>
inline ComponentID GetComponentID<CUIText>() { return CUITEXT; };\

template <>
inline ComponentID GetComponentID<CRigidBody>() { return CRIGIDBODY; };

template <>
inline ComponentID GetComponentID<CBoid>() { return CBOID; };

