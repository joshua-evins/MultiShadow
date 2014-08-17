#ifndef RENDERER_H
#define RENDERER_H
#include "../typedefs_Engine.h"
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <QtOpenGL\qglwidget>
#include <Qt\qobject.h>
#include "../ExportHeader.h"
#include "../Camera.h"
#include "../Clock.h"
#include "../BasicLight.h"
#include "DrawLayer.h"
#include "DrawStyle.h"
#include "UniformFormat.h"
#include "Renderable.h"
#include "TextureInfo.h"
#include "VAttribInfo.h"
#include "AttribBufferInfo.h"
#include "GeometryInfo.h"
#include "BufferedGeometryInfo.h"
#include "JamieFlags.h"
#include "ShaderInfo.h"
#include "BufferManager.h"
#include "LightManager.h"

namespace Engine
{
	class ENGINE_SHARED Renderer : public QGLWidget
	{
		friend struct Renderable;
		friend struct ShaderInfo;
		friend class LightManager;

	#pragma region Instance Variables
	private:
		Clock clock;
		
		int lastX;
		int lastY;
		
		uint shadowFBO;

		uint currentGeometriesIndex;
		uint currentBufferedGeometriesIndex;
		uint currentShaderProgramsIndex;
		int currentRenderablesIndex;

		BufferManager bufferManager;
		

		uint nextTextureIndex;
		uint textures[TEXTURES_MAX_CAPACITY];
		
	public:
		bool paused;
		glm::mat4 projection;
		Camera camera;
		int pointSize;
		uint depthTex;
		uint cubeMapTex;
		BasicLight light;
		
		LightManager lightManager;

		glm::mat4 baseShadowMat;
		glm::mat4 bias;

		glm::mat4 lightView;
		glm::mat4 lightProjection;
		

	#pragma endregion

		Renderer();

	#pragma region Internal Structs (with variables of that type)
	public:

		GeometryInfo geometries[GEOMETRIES_MAX_CAPACITY];

		BufferedGeometryInfo bufferedGeometries[BUFFEREDGEOMETRIES_MAX_CAPACITY];
		
		ShaderInfo shaderPrograms[SHADERPROGRAMS_MAX_CAPACITY];

		ShaderInfo* shadowShaderProgram;
		bool UseDiffuse;
		bool UseSpecular;
		int simpleNormalMap;
		bool useFancyNormals;

		Renderable renderables[RENDERABLES_MAX_CAPACITY];
	#pragma endregion
		
	Renderable* lightRenderable;

	private:
	#pragma region Override Functions
		void initializeGL();
		void paintGL();
	#pragma endregion

	private:
	#pragma region Event Handlers
		void keyPressEvent(QKeyEvent *event);
		//void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void resizeEvent(QResizeEvent*);
	#pragma endregion
	
	private:
		void compileShader(const char* fileName, GLuint shaderID);
		
	public:
	#pragma region Add Component Functions
		uint addGeometry(uint vertexBufferSize, uint indexBufferSize, uint numIndices, void* verts, void* indices,
			uint stride, DrawStyle drawStyle);
		BufferedGeometryInfo* addBufferedGeometry(uint numVerts, uint shaderID, DrawStyle drawStyle);
		ShaderInfo* addProgram(const char* vshFileName, const char* fshFileName);
		ShaderInfo* addProgram(const char* vshFileName, const char* fshFileName, const char* gshFileName);
		Renderable* addRenderable(glm::mat4& transform, uint geometryID, ShaderInfo* shaderProgramID, bool usesDepthTest = true, DrawLayer drawLayer = DrawLayer::LAYER_OPAQUE, bool hasShadow = true);
		uint addTexture(const char* fileName, const char* imageExt);

		// TODO: remove
		// less than ideal. Passing around a ShaderInfo* instead of the internally confusing ID may be better on both ends
		UniformInfo* addUniformToShader(uint shaderID, UniformFormat format, const char* name, uint dataSize, void* data);
	#pragma endregion

	public:
		void update();
		void shadowPasses();
		void drawRenderables(const glm::mat4& view, const glm::mat4& projection, bool shadowPass = false);
		void drawBufferedGeometries(const glm::mat4& view, const glm::mat4& projection);
		void initializeSize(); // Used to set the glViewport to the size of the Renderer

	private:
		bool applyUniform(uint uniformLocation, UniformInfo* info, const glm::mat4& view, const glm::mat4& viewPerspective);
		void applyStandardShaderUniforms(ShaderInfo& shader, glm::mat4& transform, const glm::mat4& view, const glm::mat4& proj, int firstOpenTextureSlot);

	#pragma region Debug
	#ifdef ENGINE_DEBUG
	public:
		Camera* debugCamera;
		bool debug_HalfScreen;
		void debug_setCamera(Camera* debugCamera);
		GeometryInfo* debug_getGeometryInfoAtIndex(uint gemoetryID);
		Renderable* debug_getRenderables();
	#endif
	#pragma endregion
	};
}
#endif