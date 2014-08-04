#pragma once
#include <glm\glm.hpp>
namespace Engine
{
	struct Material
	{
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;
		float shininess;
	};
}
