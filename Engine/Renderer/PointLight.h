#pragma once
#include <glm\glm.hpp>

namespace Engine
{
	struct PointLight
	{
		glm::vec3 facing;
		glm::mat4 view;
	};
}
