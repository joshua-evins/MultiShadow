#pragma once
#include "../typedefs_Engine.h"

namespace Engine
{
	struct BufferInfo
	{
		uint glBufferID;
		uint size;
		uint currentOffset;

		BufferInfo();
		
		bool hasSpaceFor(uint size);
		void insertData(uint size, void* data);
	private:
		void initialize(uint minimumSize);
	};
}
