#pragma once
#include <glm\glm.hpp>

namespace Engine
{
	struct BasicLight
	{
		glm::vec4 position;
		glm::vec4 diffI;
		glm::vec4 specI;
	};
}
