#include "GeometryInfo.h"

namespace Engine
{
	void GeometryInfo::initialize(uint vBufferIn, uint iBufferIn, uint vOffsetIn, uint iOffsetIn, uint numIndicesIn, uint strideIn, DrawStyle drawStyleIn)
	{
		vBuffer = vBufferIn;
		iBuffer = iBufferIn;
		vOffset = vOffsetIn;
		iOffset = iOffsetIn;

		numIndices = numIndicesIn;
		stride = strideIn;

		drawStyle = drawStyleIn;
	}
}