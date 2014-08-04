#pragma once
#include "typedefs_Engine.h"
#include <glm\glm.hpp>
struct SpotLight
{
	glm::mat4 shadowMatrix;
	glm::vec4 position;
	glm::vec4 diffI;
	glm::vec4 specI;
	uint shadowTexID;
};

