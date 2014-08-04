#pragma once
#include "../typedefs_Engine.h"
#include <glm\glm.hpp>

namespace Engine
{
	struct PointLightWithShadow
	{
		glm::mat4 shadowMatrix;
		glm::vec4 position;
		glm::vec4 diffuseColor;
		glm::vec4 specularColor;
		float shininess;
		int shadowMap;
		int active;
	};
}
