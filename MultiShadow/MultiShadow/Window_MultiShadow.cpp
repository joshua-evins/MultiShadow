#include "Window_MultiShadow.h"
#include <Renderer/Renderer.h>
#include <ShapeGenerator.h>
#include <glm\gtx\transform.hpp>
#include <iostream>
#include <LevelData.h>
#include <DebugMenuManager.h>

Window_MultiShadow::Window_MultiShadow()
{
	t = 0.0f;
	alpha = 0.5f;
	discardThreshold = 0.5f;
	useShadows = true;
	spherePosition = glm::vec3(0.0f, 1.0f, 0.0f);
	Engine::DebugMenuManager& dmm = Engine::DebugMenuManager::instance();
	dmm.attachDebugMenu(this);
	dmm.slideFloat(this, "Alpha (for the flame-whirl): ", &alpha, 0.0f, 1.0f);
	dmm.slideFloat(this, "Discard threshold (also for the flame-whirl): ", &discardThreshold, 0.0f, 1.0f);
	dmm.controlBool(this, "Enable shadows: ", &useShadows);
	dmm.controlBool(this, "Color lights: ", &usePrettyColors);
	dmm.slideVector(this, "Sphere position: ", &spherePosition[0], 3, -20.0f, 20.0f);
}

void Window_MultiShadow::initialize()
{
	renderer->camera.facing = glm::normalize(glm::vec3(-1.0f));
	renderer->camera.position = glm::vec3(1.0f);
	renderer->camera.MovementMultiplier = 1.0f;

	Engine::LevelData levelData;
	levelData.read_native("FlameMesh.bin");
	Neumont::ShapeData flameMesh = levelData.shapes[0];
	Neumont::ShapeData sphere = Neumont::ShapeGenerator::makeSphere(60);
	Neumont::ShapeData plane = Neumont::ShapeGenerator::makePlane(25);

	Engine::ShaderInfo* flameShader = renderer->addProgram("Content_Shared\\FlameMesh.vsh", "Content_Shared\\FlameMesh.fsh");
	flameShader->enableMVP("MVP");
	flameShader->enableNormalMat("NormalMat");
	timeUI = flameShader->addUniform(Engine::UniformFormat::FLOAT, "t", sizeof(float), &t);
	discardThresholdUI = flameShader->addUniform(Engine::UniformFormat::FLOAT, "discardThreshold", sizeof(float), &discardThreshold);

	Engine::ShaderInfo* shader = renderer->addProgram("Content_Shared\\MultiShadow.vsh", "Content_Shared\\MultiShadow.fsh");
	renderer->shadowShaderProgram = shader;

	shader->enableMultiShadow("PointLights", renderer->lightManager.bindPoint);

	shader->enableMVP("MVP");	
	shader->enableModelMat("ModelMat");
	shader->enableNormalMat("NormalMatrix");
	shader->enableViewMat("ViewMatrix");
	shader->enableModelView("ModelView");

	int thirtyone = 31;
	int useShadowsInt = (useShadows) ? 1 : 0;
	shader->addUniform(Engine::UniformFormat::INT, "shadowMaps", sizeof(int), &thirtyone);
	useShadowsUI = shader->addUniform(Engine::UniformFormat::INT, "useShadows", sizeof(int), &useShadowsInt);
	

	uint flameMeshGeo = renderer->addGeometry(flameMesh.vertexBufferSize(), flameMesh.indexBufferSize(), flameMesh.numIndices, flameMesh.verts, flameMesh.indices, Neumont::Vertex::STRIDE, Engine::DrawStyle::TRIANGLES);
	uint sphereGeo = renderer->addGeometry(sphere.vertexBufferSize(), sphere.indexBufferSize(), sphere.numIndices, sphere.verts, sphere.indices, Neumont::Vertex::STRIDE, Engine::DrawStyle::TRIANGLES);
	uint planeGeo = renderer->addGeometry(plane.vertexBufferSize(), plane.indexBufferSize(), plane.numIndices, plane.verts, plane.indices, Neumont::Vertex::STRIDE, Engine::DrawStyle::TRIANGLES);

	uint formattedArr[] = {
		0, Neumont::Vertex::POSITION_OFFSET, 3,
		1, Neumont::Vertex::NORMAL_OFFSET, 3,
		2, Neumont::Vertex::COLOR_OFFSET, 4
	};

	uint flameArr[] = {
		0, Neumont::Vertex::POSITION_OFFSET, 3,
		1, Neumont::Vertex::UV_OFFSET, 2,
		2, Neumont::Vertex::NORMAL_OFFSET, 3
	};

	uint noiseTex = renderer->addTexture("Content_Shared\\noise.png", "png");
	
	Engine::Renderable* flameMeshR = renderer->addRenderable(glm::scale(10.0f, 5.0f, 10.0f), flameMeshGeo, flameShader, true, Engine::DrawLayer::LAYER_BLEND);
	flameMeshR->setShaderStreamedParameters(3, flameArr);
	flameMeshR->addTexture(noiseTex, "noiseTexture");
	meshAlphaUI = flameMeshR->addUniform(Engine::UniformFormat::FLOAT, "alpha", sizeof(float), &alpha);
	

	Engine::Renderable* planeR = renderer->addRenderable(glm::mat4(), planeGeo, shader);
	planeR->setShaderStreamedParameters(3, formattedArr);

	sphereR = renderer->addRenderable(glm::translate(0.0f, 1.0f, 0.0f), sphereGeo, shader);
	sphereR->setShaderStreamedParameters(3, formattedArr);

	Engine::ShaderInfo* lightShader = renderer->addProgram("Content_Shared\\LightShader.vsh", "Content_Shared\\LightShader.fsh");
	lightShader->enableMVP("MVP");
	for(int i=0; i<4; i++)
	{
		lightRs[i] = renderer->addRenderable(glm::mat4(2.0f), sphereGeo, lightShader, true, Engine::DrawLayer::LAYER_OPAQUE, false);
		lightRs[i]->setShaderStreamedParameter(0, Neumont::Vertex::POSITION_OFFSET, 3);
		lightRs[i]->addUniform(Engine::UniformFormat::GLMVEC4, "LightColor", sizeof(glm::vec4), &renderer->lightManager.getColorOfLight(i));
	}
}


void Window_MultiShadow::inheritedUpdate()
{
	t += 0.01f;
	timeUI->updateInfo(sizeof(float), &t);
	meshAlphaUI->updateInfo(sizeof(float), &alpha);
	int useShadowsInt = (useShadows) ? 1 : 0;
	useShadowsUI->updateInfo(sizeof(int), &useShadowsInt);
	discardThresholdUI->updateInfo(sizeof(float), &discardThreshold);

	sphereR->transform = glm::translate(spherePosition);

	if(usePrettyColors)
		renderer->lightManager.setAllColorsDefault();
	else
		renderer->lightManager.setAllColorsWhite();

	for(int i=0; i<4; i++)
	{
		glm::vec4 color(renderer->lightManager.getColorOfLight(i));
		lightRs[i]->uniforms[0].updateInfo(sizeof(glm::vec4), &color);
		lightRs[i]->transform = glm::translate(renderer->lightManager.getPositionOfLight(i));
	}
}