#include "BufferManager.h"
#include <GL\glew.h>
#include <iostream>

namespace Engine
{
	BufferManager::BufferManager()
	{
		currentBuffer = 0;
	}

	BufferInsertionRecord BufferManager::addData(uint size, void* data)
	{
		BufferInfo* buffer = &buffers[currentBuffer];

		if(!buffer->hasSpaceFor(size))
			buffer = &buffers[++currentBuffer];
		
		uint initialOffset = buffer->currentOffset;
		buffer->insertData(size, data);
		
		BufferInsertionRecord bir;
		bir.glBufferID = buffer->glBufferID;
		bir.offset = initialOffset;
		bir.size = size;

		return bir;
	}
	
	void BufferManager::setOffsetToMultipleOf(uint snapValue)
	{
		buffers[currentBuffer].currentOffset += snapValue - (buffers[currentBuffer].currentOffset % snapValue);
	}

}