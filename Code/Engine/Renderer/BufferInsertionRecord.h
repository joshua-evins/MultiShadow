#pragma once
#include "../typedefs_Engine.h"
#include "BufferInfo.h"

namespace Engine
{
	struct BufferInsertionRecord
	{
		uint glBufferID;
		uint size;
		uint offset;
	};
}
