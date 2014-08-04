#pragma once
#include "../typedefs_Engine.h"
#include "../ExportHeader.h"

namespace Engine
{
	struct ENGINE_SHARED AttribBufferInfo
	{
		uint buffer;
		uint size;
		uint glType;
		uint stride;
	};
}
