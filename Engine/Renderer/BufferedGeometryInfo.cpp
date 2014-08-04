#include "BufferedGeometryInfo.h"
#include <cassert>
#include <GL\glew.h>

namespace Engine
{
	BufferedGeometryInfo::BufferedGeometryInfo()
	{
		numAttribs = 0;
		numUniforms = 0;
	}

	uint BufferedGeometryInfo::addAttribBuffer(uint numGlTypes, uint glType, uint stride, void* data, uint dataSize, uint usageHint)
	{
		AttribBufferInfo& attrib = attribs[numAttribs];
		glGenBuffers(1, &attrib.buffer);
		glBindBuffer(GL_ARRAY_BUFFER, attrib.buffer);
		glBufferData(GL_ARRAY_BUFFER, dataSize, data, usageHint);

		attrib.size = numGlTypes;
		attrib.glType = glType;
		attrib.stride = stride;

		numAttribs++;
		return attrib.buffer;
	}

	uint BufferedGeometryInfo::addAttribBuffer(uint numGlTypes, uint glType, uint stride, void* data, uint dataSize)
	{
		return addAttribBuffer(numGlTypes, glType, stride, data, dataSize, GL_STREAM_DRAW);
	}

	UniformInfo* BufferedGeometryInfo::addUniform(UniformFormat format, const char* name, uint dataSize, void* data)
	{
		assert(numUniforms < UNIFORMS_MAX_CAPACITY );

		uniforms[numUniforms].initialize(format, name, dataSize, data);
		return &(uniforms[numUniforms++]);
	}
}