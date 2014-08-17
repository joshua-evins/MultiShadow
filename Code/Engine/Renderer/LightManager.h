#pragma once
#include "../typedefs_Engine.h"
#include "../ExportHeader.h"
#include <glm\glm.hpp>
#include "PointLight.h"
#include "PointLightGLSL.h"
#include "UniformBlockInfo.h"
#include "BufferInsertionRecord.h"

namespace Engine
{
	class Renderer;
	struct ShaderInfo;
	class ENGINE_SHARED LightManager 
	{
		friend class Renderer;
		Renderer* renderer;
		ShaderInfo* lightShader;

		int activeLights;
		int currentLightID;
		PointLight* currentLightData;
		PointLightGLSL* currentLight;

		glm::mat4 projection;
		glm::mat4 projectionBias;
		glm::vec3 up;
		
		PointLight pointLightsRawData[4];
		PointLightGLSL pointLights[4];

		UniformBlockInfo gpuData;
		BufferInsertionRecord bir;

		
		void updateView();
		void updateShadowMatrix();
		void updateMatrices();

	public:
		uint shadowMapArray;
		int lightTextureBindPoint;
		void initialize();
		int bindPoint;
		bool initialized;
		int mapWidth;
		int mapHeight;
		LightManager(int numStartingLights, Renderer* renderer);

		void setCurrentLight(int targetLight);
		void bindToGL();
		void update();

		void setPosition(glm::vec3& newPos);
		void moveLight(glm::vec3& deltaPos);
		void setLightDirection(glm::vec3& newDir);
		void rotateLight(glm::mat4& deltaRotation); // rotate facing, rebuild shadowMatrix and position
		void setDiffuse(glm::vec4& newDiffuse);
		void setSpecular(glm::vec4& newSpecular);
		void setActive(bool active);
		void writeAllData();
		void updateAllMatrices();
		void setAllColorsDefault();
		void setAllColorsWhite();
		glm::vec4 getColorOfLight(int index);
		glm::vec3 getPositionOfLight(int index);

		// TODO: move these
		bool vec3Equal(glm::vec3& a, glm::vec3& b);
		bool vec4Equal(glm::vec4& a, glm::vec4& b);
	};
}