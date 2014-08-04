#pragma once
#include "../typedefs_Engine.h"
#include "../ExportHeader.h"
#include "AttribBufferInfo.h"
#include "DrawStyle.h"
#include "UniformInfo.h"

namespace Engine
{
	struct ENGINE_SHARED BufferedGeometryInfo
	{
		BufferedGeometryInfo();

		uint shaderID;
		uint numVerts;
		uint numAttribs;
		AttribBufferInfo attribs[BUFFEREDGEOMETRY_MAX_ATTRIBS];
		DrawStyle drawStyle;

		uint numUniforms;
		UniformInfo uniforms[UNIFORMS_MAX_CAPACITY];

		uint addAttribBuffer(uint numGlTypes, uint glType, uint stride, void* data, uint dataSize, uint usageHint);
		uint addAttribBuffer(uint numGlTypes, uint glType, uint stride, void* data, uint dataSize);

		UniformInfo* addUniform(UniformFormat format, const char* name, uint dataSize, void* data);
	};
}