#include "UniformInfo.h"
#include <cstring>
#include <cassert>

namespace Engine
{
	void UniformInfo::initialize(UniformFormat formatIn, const char* nameIn, uint dataSize, void* dataIn)
	{
		assert (NAME_MAX_CAPACITY >= strlen(nameIn));
		memset(name, '\0', NAME_MAX_CAPACITY);
		memcpy(name, nameIn, strlen(nameIn));

		format = formatIn;
		memcpy(data, dataIn, dataSize);
	}

	void UniformInfo::updateInfo( uint dataSize, void* dataIn)
	{
		memcpy(data, dataIn, dataSize);
	}
}
