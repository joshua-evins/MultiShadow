#include "BufferInfo.h"
#include <GL\glew.h>
#include <iostream>

namespace Engine
{
	BufferInfo::BufferInfo()
	{
		glBufferID = UINT_MAX;
		size = UINT_MAX;
		currentOffset = 0;
	}

	void BufferInfo::initialize(uint minimumSize)
	{
		size = (minimumSize > BUFFER_SIZE) ? minimumSize : BUFFER_SIZE;

		glGenBuffers(1, &glBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, glBufferID);
		glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	}

	bool BufferInfo::hasSpaceFor(uint size)
	{
		return this->size - currentOffset > size;
	}

	// note: "size" is also a member of BufferInfo. 
	//		 use this->size to differentiate between the two
	void BufferInfo::insertData(uint size, void* data)
	{
		if(glBufferID == UINT_MAX)
			initialize(size);

		glBindBuffer(GL_ARRAY_BUFFER, glBufferID);
		glBufferSubData(GL_ARRAY_BUFFER, currentOffset, size, data);
		currentOffset += size;
	}

	
}