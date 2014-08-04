#pragma once
#include "../typedefs_Engine.h"
#include "../ExportHeader.h"
#include "UniformInfo.h"
#include "UniformBlockInfo.h"
#define NULL 0

namespace Engine
{
	class Renderer;
	struct ENGINE_SHARED ShaderInfo
	{
		uint programID;
		uint vertexID;
		uint fragmentID;
		uint geometryShaderID;

		// seems unused
		void initialize(uint programIDIn, uint vertexIDIn, uint fragmentIDIn, uint geometryShaderIDIn);
		int getUniformLocation(const char* nameInShader);
		UniformInfo* addUniform(UniformFormat format, const char* name, uint dataSize, void* data);
		void enableMVP(const char* nameInShader);
		void enableModelView(const char* nameInShader);
		void enableNormalMat(const char* nameInShader);
		void enableLight(const char* lightPosition_NameInShader, const char* lightDiffuseIntensity_NameInShader = NULL, const char* lightSpecularIntensity_NameInShader = NULL);
		void enableShadow(const char* shadowMat_NameInShader, const char* shadowMap_NameInShader);
		void enableCubeMap(const char* cubeMap_NameInShader);
		void enableCameraPosWorld(const char* cameraPosWorld_NameInShader);
		void enableModelMat(const char* modelMat_NameInShader);
		void enableViewMat(const char* viewMat_NameInShader);
		void enableMultiShadow(const char* multiShadow_NameInShader, uint lightManagerBindingPoint);
		void enableViewProjection(const char* viewProjection_NameInShader);
		uint numUniforms;
		UniformInfo uniforms[SHADERINFO_UNIFORMS_MAX_CAPACITY];

		uint numUniformBlocks;
		UniformBlockInfo uniformBlocks[4];
		UniformBlockInfo* addUniformBlock(const char* name, uint objectSize, uint numObjects, void* data, Renderer* parent);

		bool sendMVP;
		int mvpUL;
			
		bool sendModelView;
		int modelViewUL;
		
		bool sendNormalMat;
		int normalMatUL;

		bool sendLight;
		int lightPositionUL;
		int lightDiffuseIntensityUL;
		int lightSpecularIntensityUL;
			
		bool sendShadow;
		int shadowMatUL;
		int shadowMapUL;

		bool sendCubeMap;
		int cubeMapUL;

		bool sendCameraPosWorld;
		int cameraPosWorldUL;

		bool sendModelMat;
		int modelMatUL;

		bool sendViewProjection;
		int viewProjectionUL;

		bool sendViewMat;
		int viewMatUL;
		ShaderInfo();	
	};
}