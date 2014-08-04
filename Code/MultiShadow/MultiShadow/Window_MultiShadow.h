#pragma once
#include <RendererWindow.h>
#include <glm\glm.hpp>

namespace Engine
{ 
	struct UniformInfo;
	struct Renderable;
}

class Window_MultiShadow : public Engine::RendererWindow
{
public:
	Engine::UniformInfo* timeUI;
	Engine::UniformInfo* meshAlphaUI;
	Engine::UniformInfo* useShadowsUI;
	Engine::UniformInfo* discardThresholdUI;
	Engine::Renderable* sphereR;
	Engine::Renderable* lightRs[4];

	float t;
	float alpha;
	float discardThreshold;
	bool useShadows;
	bool usePrettyColors;
	glm::vec3 spherePosition;
	Window_MultiShadow();
	virtual void initialize();
	virtual void inheritedUpdate();

};
