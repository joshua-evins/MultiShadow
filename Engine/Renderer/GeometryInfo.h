#pragma once
#include "../typedefs_Engine.h"
#include "../ExportHeader.h"
#include "DrawStyle.h"

namespace Engine
{
	struct ENGINE_SHARED GeometryInfo
	{
		uint vBuffer;
		uint iBuffer;

		uint vOffset;
		uint iOffset;

		uint numIndices;

		uint stride;

		DrawStyle drawStyle;
			
		void initialize(uint vBuffer, uint iBuffer, uint vOffset, uint iOffset, uint numIndices, uint stride, DrawStyle drawStyle);
	};
}
