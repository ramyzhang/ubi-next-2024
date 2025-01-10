#pragma once

#include <memory>
#include "engine/graphics/Primitives.h"

struct CMesh {
	// only store a pointer to the mesh location, we don't wanna copy it over and over
	std::shared_ptr<Model> model;
};

// TODO: make this specific to the models that will be used in the game soon