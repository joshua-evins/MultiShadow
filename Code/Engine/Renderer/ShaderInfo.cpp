#include "ShaderInfo.h"
#include <GL\glew.h>
#include <cassert>
#include "Renderer.h"
#include "BufferManager.h"
#include <iostream>
#include <fstream>

namespace Engine
{
	ShaderInfo::ShaderInfo()
	{
		sendLight = false;
		sendModelView = false;
		sendMVP = false;
		sendNormalMat = false;
		sendShadow = false;
		sendCubeMap = false;
		sendCameraPosWorld = false;
		sendViewMat = false;
		sendViewProjection = false;
		numUniformBlocks = 0;
	}

	void ShaderInfo::initialize(uint programIDIn, uint vertexIDIn, uint fragmentIDIn, uint geometryShaderIDIn)
	{
		programID = programIDIn;
		vertexID = vertexIDIn;
		fragmentID = fragmentIDIn;
		geometryShaderID = geometryShaderIDIn;

		sendMVP = false;
		sendModelView = false;
		sendNormalMat = false;
		sendLight = false;
		sendShadow = false;
	}

	int ShaderInfo::getUniformLocation(const char* nameInShader)
	{
		int UL = glGetUniformLocation( programID, nameInShader); 
		return UL;
	}

	// clone of Renderable::addUniform
	UniformInfo* ShaderInfo::addUniform(UniformFormat formatIn, const char* name, uint dataSize, void* data)
	{
		assert(numUniforms < SHADERINFO_UNIFORMS_MAX_CAPACITY );

		uniforms[numUniforms].initialize(formatIn, name, dataSize, data);
		return &(uniforms[numUniforms++]);
	}

	UniformBlockInfo* ShaderInfo::addUniformBlock(const char* name, uint objectSize, uint numObjects, void* data, Renderer* parent)
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

		//parent->insertIntoOneBuffer(GL_UNIFORM_BUFFER, parent->nextUniformOffset, &(parent->currentUniformBufferIndex),
		//	UNIFORMBUFFERIDS_MAX_CAPACITY, numObjects*objectSize, parent->uniformBufferIDs, data);

		uniformBlocks[numUniformBlocks].initialize(objectSize, numObjects,
			bir.glBufferID, bir.offset);

		UniformBlockInfo* currentBlock = &(uniformBlocks[numUniformBlocks]);

		// There is no glGenUniformBlockBindingPoint for some reason or another. You have to define your own ids for it.
		currentBlock->bindPoint = parent->nextAvailableUniformBlockBindingPoint;
		// This value is just for the convenience of not updating the code to reflect the change of storing bindPoint inside the currentBlock
		uint bindingPoint = currentBlock->bindPoint;
		
		// Typical uniformLocation query
		currentBlock->uniformLocation = glGetUniformBlockIndex(programID, name);

		// Tell programID that uniformLocation should look at bindingPoint for its data
		// This is the function that needs to be called in the draw loop so that each uniform block is looking where the Shader or Renderable wants it to look
		// Take note that even though the binding point hasn't been given any data we can still tell the shader to use it. It's a "pointer", more or less.
		glUniformBlockBinding(programID, currentBlock->uniformLocation, bindingPoint);

		// Below is where the assignment of data to the binding point happens

		// Typical buffer bind call because OpenGL requires them for some reason or another
		glBindBuffer(GL_UNIFORM_BUFFER, currentBlock->uBuffer);

		// (The first parameter is redundant, just make sure uBuffer is bound to GL_UNIFORM_BUFFER)
		// Tell bindingPoint that it should reference the data in the buffer object uBuffer starting at uBufferOffset and going for blockSize bytes
		// The first parameter here is misleading. It is setting metadata on bindingPoint, not GL_UNIFORM_BUFFER
		glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, currentBlock->uBuffer, currentBlock->uBufferOffset, blockSize);


		// Debugging function below this wall of comments
		// C++ packs data to N4 (but not necessarily the same way for each compiler)
			// Becase of this your class/struct may not have the data footprint you expect, which is important here
			// GLSL structs always assume tightly-packed data, so if your C++ struct has any padding at all you will have alignment issues
			// In a similar problem, the compiler may also reorder members of your class/struct in order to pack the data more tightly
		// The below code writes the data in the Uniform Block to a file for inspection
			// Packing is pretty obvious because it is usually assigned a pattern visible in a hex editor (something like CD, FE, etc.)
			// Reorded member variables are not always so easy to detect, hence why I manually inspect the data
	

		//char* buf = new char[blockSize];
		//glGetBufferSubData(GL_UNIFORM_BUFFER, bir.offset, blockSize, buf);
		//
		////if(parent->nextAvailableUniformBlockBindingPoint == 2)
		//{
		//std::ofstream out("rawOutput", std::ios::out | std::ios::binary);
		//
		//out.write(buf, blockSize);
		//out.close();
		//}

		//////////////////////////////////// End buffer save function
		
		numUniformBlocks++;

		parent->nextAvailableUniformBlockBindingPoint++;
		
		return currentBlock;
	}

	void ShaderInfo::enableMVP(const char* nameInShader)
	{
		sendMVP = true;
		mvpUL = getUniformLocation(nameInShader);
	}

	void ShaderInfo::enableModelView(const char* nameInShader)
	{
		sendModelView = true;
		modelViewUL = getUniformLocation(nameInShader);
	}

	void ShaderInfo::enableNormalMat(const char* nameInShader)
	{
		sendNormalMat = true;
		normalMatUL = getUniformLocation(nameInShader);
	}

	void ShaderInfo::enableLight(const char* lightPosition_NameInShader, const char* lightDiffuseIntensity_NameInShader, const char* lightSpecularIntensity_NameInShader)
	{
		sendLight = true;
		lightPositionUL = getUniformLocation(lightPosition_NameInShader);

		if(lightDiffuseIntensity_NameInShader != NULL)
			lightDiffuseIntensityUL = getUniformLocation(lightDiffuseIntensity_NameInShader);
		else
			lightDiffuseIntensityUL = -1;

		if(lightSpecularIntensity_NameInShader != NULL)
			lightSpecularIntensityUL = getUniformLocation(lightSpecularIntensity_NameInShader);
		else
			lightSpecularIntensityUL = -1;
	}
	
	void ShaderInfo::enableShadow(const char* shadowMat_NameInShader, const char* shadowMap_NameInShader)
	{
		sendShadow = true;
		shadowMatUL = getUniformLocation(shadowMat_NameInShader);
		shadowMapUL = getUniformLocation(shadowMap_NameInShader);
	}

	void ShaderInfo::enableCubeMap(const char* cubeMap_NameInShader)
	{
		sendCubeMap = true;
		cubeMapUL = glGetUniformLocation(programID, cubeMap_NameInShader);
	}

	void ShaderInfo::enableCameraPosWorld(const char* cameraPosWorld_NameInShader)
	{
		sendCameraPosWorld = true;
		cameraPosWorldUL = glGetUniformLocation(programID, cameraPosWorld_NameInShader);
	}

	void ShaderInfo::enableModelMat(const char* modelMat_NameInShader)
	{
		sendModelMat = true;
		modelMatUL = getUniformLocation(modelMat_NameInShader);
	}

	void ShaderInfo::enableViewMat(const char* viewMat_NameInShader)
	{
		sendViewMat = true;
		viewMatUL = getUniformLocation(viewMat_NameInShader);
	}

	void ShaderInfo::enableMultiShadow(const char* multiShadow_NameInShader, uint lightManagerBindingPoint)
	{
		uint UL = glGetUniformBlockIndex(programID, multiShadow_NameInShader);

		glUniformBlockBinding(programID, UL, lightManagerBindingPoint);
	}

	void ShaderInfo::enableViewProjection(const char* viewProjection_NameInShader)
	{
		sendViewProjection = true;
		viewProjectionUL = getUniformLocation(viewProjection_NameInShader);
	}
}