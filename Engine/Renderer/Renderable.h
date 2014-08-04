#pragma once
#include "../typedefs_Engine.h"
#include "../ExportHeader.h"
#include <GL\glew.h>
#include "DrawLayer.h"
#include "UniformInfo.h"
#include "UniformBlockInfo.h"

#include "TextureInfo.h"
#include "VAttribInfo.h"

#include <glm/glm.hpp>

namespace Engine
{
	class Renderer;
	struct ShaderInfo;
	struct ENGINE_SHARED Renderable
	{
		glm::mat4 transform;
		bool visible;
		DrawLayer drawLayer;
		Renderer* parent;
		
		int jamie;

		uint geometryID;
		ShaderInfo* shader;

		uint numUniforms;
		UniformInfo uniforms[UNIFORMS_MAX_CAPACITY];
		
		uint numUniformBlocks;
		UniformBlockInfo uniformBlocks[UNIFORMBLOCKS_MAX_CAPACITY];
		
		uint numTextures;
		TextureInfo textureInfos[TEXTUREINFOS_MAX_CAPACITY];

		bool useDepthTest;
		bool hasShadow;
		
		uint numVAttribs;
		VAttribInfo vAttribs[ATTRIBS_MAX_CAPACITY];

		void initialize(glm::mat4& transform, Renderer* parent, uint geometryID, ShaderInfo* shader, bool visible, bool usesDepthTest, DrawLayer drawLayer = DrawLayer::LAYER_OPAQUE, bool hasShadow = true);
		UniformInfo* addUniform(UniformFormat format, const char* name, uint dataSize, void* data);
		uint addTexture(uint textureID, const char* nameInShader);
		UniformBlockInfo* addUniformBlock(const char* name, uint objectSize, uint numObjects, void* data);
		void setJamieFlags(int flags);
		void setShaderStreamedParameter(uint layoutLocation, uint offset, uint numElements, uint element = GL_FLOAT);
		void setShaderStreamedParameters(uint numAttribs, uint* formattedArray__layout_Offset_NumElements_perAttrib);
	};
}
