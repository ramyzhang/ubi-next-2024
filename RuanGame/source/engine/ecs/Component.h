#pragma once

#include "game/components/ComponentIncludes.h"

enum ComponentID {
	CTRANSFORM,
	CMESH
};

#define GET_COMPONENT_TYPE(type) \
    ((type) == CTRANSFORM ? CTransform : \
     (type) == CMESH ? CMesh : void)
