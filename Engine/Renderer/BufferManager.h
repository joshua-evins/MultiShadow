#pragma once
#include "../typedefs_Engine.h"
#include "BufferInfo.h"
#include "BufferInsertionRecord.h"

namespace Engine
{
	class BufferManager
	{
		uint currentBuffer;
		BufferInfo buffers[NUM_BUFFERS];

	public:
		BufferManager();
		BufferInsertionRecord addData(uint size, void* data);
		void setOffsetToMultipleOf(uint snapValue);
	};
}
