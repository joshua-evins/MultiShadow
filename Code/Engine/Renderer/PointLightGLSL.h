#pragma once
#include <glm\glm.hpp>

namespace Engine
{
	struct PointLightGLSL
	{
		glm::mat4 shadowMatrix;
		glm::vec4 positionWorld;
		glm::vec4 diffuseColor;
		glm::vec4 specularColor;
		int shadowMap;
		int active;
	private:
		int padding1;
		int padding2;
	};
}
