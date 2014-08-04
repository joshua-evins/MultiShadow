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

		// There is no glGenUniformBlockBindingPoint for some reason or another. You have to define your own ids for it.
		currentBlock->bindPoint = parent->nextAvailableUniformBlockBindingPoint;
		
		// Typical-ish uniformLocation query
		currentBlock->uniformLocation = glGetUniformBlockIndex(programID, name);

		// Tell programID that uniformLocation should look at bindingPoint for its data
		// This is the function that needs to be called in the draw loop so that each uniform block is looking where the Shader or Renderable wants it to look
		// Take note that even though the binding point hasn't been given any data we can still tell the shader to use it. It's a "pointer", more or less.
		glUniformBlockBinding(programID, currentBlock->uniformLocation, currentBlock->bindPoint);

		// Below is where the assignment of data to the binding point happens

		// Typical buffer bind call because OpenGL requires them for some reason or another
		glBindBuffer(GL_UNIFORM_BUFFER, currentBlock->uBuffer);

		// (The first parameter is redundant, just make sure uBuffer is bound to GL_UNIFORM_BUFFER)
		// Tell bindingPoint that it should reference the data in the buffer object uBuffer starting at uBufferOffset and going for blockSize bytes
		// The first parameter here is misleading. It is setting metadata on bindingPoint, not GL_UNIFORM_BUFFER
		glBindBufferRange(GL_UNIFORM_BUFFER, currentBlock->bindPoint, currentBlock->uBuffer, currentBlock->uBufferOffset, blockSize);


		// Debugging function below this wall of comments
		// C++ packs data to N4 (but not necessarily the same way for each compiler)
			// Becase of this your class/struct may not have the data footprint you expect, which is important here
			// GLSL structs always assume tightly-packed data, so if your C++ struct has any padding at all you will have alignment issues
			// In a similar problem, the compiler may also reorder members of your class/struct in order to pack the data more tightly
		// The below code writes the data in the Uniform Block to a file for inspection
			// Packing is pretty obvious because it is usually assigned a pattern visible in a hex editor (something like CD, FE, etc.)
			// Reorded member variables are not always so easy to detect, hence why I manually inspect the data
	

		//char* buf = new char[chunkSize];
		//glGetBufferSubData(GL_UNIFORM_BUFFER, parent->nextUniformOffset[parent->currentUniformBufferIndex], chunkSize, buf);

		//if(parent->nextAvailableUniformBlockBindingPoint == 2)
		//{
		//std::ofstream out("rawOutput", std::ios::out | std::ios::binary);
		//
		//out.write(reinterpret_cast<char*>(&x), sizeof(uint));
		//x = currentBlock->uBufferOffset;
		//out.write(reinterpret_cast<char*>(&x), sizeof(uint));
		//x = GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT;
		//out.write(reinterpret_cast<char*>(&x), sizeof(uint));
		//out.close();
		//}

		//////////////////////////////////// End buffer save function
		
		numUniformBlocks++;

		parent->nextAvailableUniformBlockBindingPoint++;
		
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