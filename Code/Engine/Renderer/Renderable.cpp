#include "Renderable.h"
#include <GL\glew.h>
#include "JamieFlags.h"
#include "ShaderInfo.h"
#include "GeometryInfo.h"
#include "Renderer.h"

namespace Engine
{
	class Renderer;
	void Renderable::initialize(glm::mat4& transform, Renderer* parent, uint geometryIDIn, ShaderInfo* shader, bool visibleIn, bool usesDepthTest, DrawLayer drawLayer, bool hasShadow)
	{
		this->transform = transform;
		this->parent = parent;
		this->drawLayer = drawLayer;
		geometryID = geometryIDIn;
		this->shader = shader;

		numUniforms = 0;
		numUniformBlocks = 0;
		numTextures = 0;

		visible = visibleIn;
		useDepthTest = usesDepthTest;
		this->hasShadow = hasShadow;
		jamie = JamieFlags::JAMIE_NOFLAGS;

		numVAttribs = 0;
	}

	UniformInfo* Renderable::addUniform(UniformFormat formatIn, const char* name, uint dataSize, void* data)
	{
		assert(numUniforms < UNIFORMS_MAX_CAPACITY );

		uniforms[numUniforms].initialize(formatIn, name, dataSize, data);
		return &(uniforms[numUniforms++]);
	}

	uint Renderable::addTexture(uint textureID, const char* nameInShader)
	{		
		TextureInfo& tex = textureInfos[numTextures];
		tex.textureID = textureID;
		tex.uniformLoc = glGetUniformLocation(shader->programID, nameInShader);
		return numTextures++;
	}

	UniformBlockInfo* Renderable::addUniformBlock(const char* name, uint objectSize, uint numObjects, void* data)
	{
		assert( objectSize * numObjects <= GL_MAX_UNIFORM_BLOCK_SIZE);
		
		uint blockSize = objectSize * numObjects;


		int offsetVal;

		// A uniform block MUST start at a multiple of whatever this query returns. (uniformBlockOffset % offsetVal == 0) should be true. 
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &offsetVal);

		// This function snaps the buffer offset forward to a multiple of offsetVal
		parent->bufferManager.setOffsetToMultipleOf(offsetVal);

		// It won't matter if the offset was set past the end of the buffer because this call will move to the next buffer anyway (and start at 0, a multiple of everything)
		BufferInsertionRecord bir = parent->bufferManager.addData(blockSize, data);

		uniformBlocks[numUniformBlocks].initialize(objectSize, numObjects,
			bir.glBufferID, bir.offset);

		UniformBlockInfo* currentBlock = &(uniformBlocks[numUniformBlocks]);
		
		uint programID = shader->programID;
		
		// Typical-ish uniformLocation query
		currentBlock->uniformLocation = glGetUniformBlockIndex(programID, name);

		// Tell programID that uniformLocation should look at bindingPoint for its data
		// This is the function that needs to be called in the draw loop so that each uniform block is looking where the Shader or Renderable wants it to look
		// Take note that even though the binding point hasn't been given any data we can still tell the shader to use it. It's a "pointer", more or less.
		glUniformBlockBinding(programID, currentBlock->uniformLocation, currentBlock->bindPoint);
		
		numUniformBlocks++;
		
		return currentBlock;
	}

	void Renderable::setJamieFlags(int flags)
	{
		jamie = flags;
	}

	void Renderable::setShaderStreamedParameter(uint layoutLocation, uint offset, uint numElements, uint element)
	{
		GeometryInfo& geometry = parent->geometries[geometryID];
		glEnableVertexAttribArray(layoutLocation);
		glBindBuffer(GL_ARRAY_BUFFER, geometry.vBuffer);
		vAttribs[numVAttribs].location = layoutLocation;
		vAttribs[numVAttribs].size = numElements;
		vAttribs[numVAttribs].element = element;
		vAttribs[numVAttribs++].offset = offset;

		glVertexAttribPointer(layoutLocation, numElements, element, GL_FALSE, geometry.stride, BUFFER_OFFSET(offset));
	}

	void Renderable::setShaderStreamedParameters(uint numAttribs, uint* formattedArray__layout_Offset_NumElements_perAttrib)
	{
		uint* arrIn = formattedArray__layout_Offset_NumElements_perAttrib;
		for(int i=0; i<numAttribs; i++)
			setShaderStreamedParameter(arrIn[3*i], arrIn[3*i+1], arrIn[3*i+2]);
	}
}