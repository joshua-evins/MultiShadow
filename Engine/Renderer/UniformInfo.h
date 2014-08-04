#pragma once
#include "../typedefs_Engine.h"
#include "../ExportHeader.h"
#include "UniformFormat.h"
namespace Engine
{
	struct ENGINE_SHARED UniformInfo
	{
		UniformFormat format;
		char name[NAME_MAX_CAPACITY];
		char data[UNIFORMDATA_MAX_CAPACITY];

		void updateInfo(uint dataSize, void* data);
		void initialize(UniformFormat formatIn, const char* nameIn, uint dataSize, void* dataIn);
	};
}
