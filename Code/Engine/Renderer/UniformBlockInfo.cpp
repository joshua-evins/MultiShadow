#include "UniformBlockInfo.h"
#include <GL\glew.h>
#include <iostream>

namespace Engine
{
	uint UniformBlockInfo::nextAvailableUniformBlockBindingPoint = 0;

	UniformBlockInfo::UniformBlockInfo()
	{
		this->uBuffer = UINT_MAX;
	}

	void UniformBlockInfo::initialize(uint objectSize, uint numObjects, uint uBuffer, uint uBufferOffset)
	{
		this->objectSize = objectSize;
		this->numObjects = numObjects;
		this->uBuffer = uBuffer;
		this->uBufferOffset = uBufferOffset;
		bindPoint = nextAvailableUniformBlockBindingPoint++;

		glBindBuffer(GL_UNIFORM_BUFFER, uBuffer);
		glBindBufferRange(GL_UNIFORM_BUFFER, bindPoint, uBuffer, uBufferOffset, objectSize * numObjects);
	}

	void UniformBlockInfo::getObjectCopy(uint indexOfObject, void* objectBuffer)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, uBuffer);
		uint offsetIntoBlock = objectSize * indexOfObject;
		glGetBufferSubData(GL_UNIFORM_BUFFER, uBufferOffset + offsetIntoBlock, objectSize, objectBuffer);
	}

	void UniformBlockInfo::updateObject(uint indexOfObject, void* newObjectData)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, uBuffer);
		uint offsetIntoBlock = objectSize * indexOfObject;
		glBufferSubData(GL_UNIFORM_BUFFER, uBufferOffset + offsetIntoBlock, objectSize, newObjectData);
	}

	void UniformBlockInfo::getMemberCopy(uint indexOfObject, uint offsetIntoObject, uint size, void* data)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, uBuffer);
		uint offsetIntoBlock = objectSize * indexOfObject + offsetIntoObject;
		glGetBufferSubData(GL_UNIFORM_BUFFER, uBufferOffset + offsetIntoBlock, size, data);
	}

	void UniformBlockInfo::updateMember(uint indexOfObject, uint offsetIntoObject, uint size, void* data)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, uBuffer);
		uint offsetIntoBlock = objectSize * indexOfObject + offsetIntoObject;
		glBufferSubData(GL_UNIFORM_BUFFER, uBufferOffset + offsetIntoBlock, size, data);
	}

	void UniformBlockInfo::fullUpdate(void* data)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, uBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, uBufferOffset, objectSize * numObjects, data);
	}
}