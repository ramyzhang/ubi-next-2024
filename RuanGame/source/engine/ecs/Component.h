#pragma once

#include "components/ComponentIncludes.h"

enum ComponentID {
	CTRANSFORM,
	CMESH
};

template <typename T>
inline ComponentID GetComponentID() { return -1; };

template <>
inline ComponentID GetComponentID<CTransform>() { return CTRANSFORM; };

template <>
inline ComponentID GetComponentID<CMesh>() { return CMESH; };
