#pragma once
#include "../typedefs_Engine.h"
#include "../ExportHeader.h"

namespace Engine
{
	struct ENGINE_SHARED UniformBlockInfo
	{
		static uint nextAvailableUniformBlockBindingPoint;
		uint uBuffer;
		uint uBufferOffset;
		uint objectSize;
		uint numObjects;
		uint uniformLocation;
		uint bindPoint;

		UniformBlockInfo();

		void initialize(uint objectSize, uint numObjects, uint uBuffer, uint uBufferOffset);
		void getObjectCopy(uint indexOfObject, void* objectBuffer);
		void updateObject(uint indexOfObject, void* newObjectData);

		void getMemberCopy(uint indexOfObject, uint offsetIntoObject, uint size, void* data);
		void updateMember(uint indexOfObject, uint offsetIntoObject, uint size, void* data);
		void fullUpdate(void* data);
	};
}
