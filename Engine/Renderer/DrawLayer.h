#pragma once
#include "../typedefs_Engine.h"
namespace Engine
{
	enum DrawLayer
	{
		LAYER_OPAQUE = 0,
		LAYER_BLEND = 1,
		DRAWLAYER_COUNT = 2 // must be updated when a new layer is added
	};
}