#pragma once
#include "../Renderer/Renderer.h"
#include "../ExportHeader.h"
#include "glm\glm.hpp"


namespace Engine
{
namespace Editing
{
	struct Node;
	struct ENGINE_SHARED Character
	{
		Character();
		
		glm::vec3 position;
		glm::vec3 facing;
		float speed;

		uint flagNodeIndex;
		uint enemyFlagNodeIndex;

		Renderable* renderable;
		UniformInfo* transform;

		Node** nodePath;
		uint totalNodes;
		uint nextNode;

		void face(glm::vec3 thisWay);
		void startPath(Node** newNodes, uint numNodes);
		bool path();
		void faster();
		void slower();
	};
}
}