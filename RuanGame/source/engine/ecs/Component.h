#pragma once

#include "components/ComponentIncludes.h"

enum ComponentID {
	CTRANSFORM,
	CMESH,
	CCOLLIDER,
	CUISPRITE,
	CUITEXT
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
inline ComponentID GetComponentID<CUISprite>() { return CUISPRITE; };

template <>
inline ComponentID GetComponentID<CUIText>() { return CUITEXT; };
