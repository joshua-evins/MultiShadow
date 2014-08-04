#include "LightManager.h"
#include <glm\gtx\transform.hpp>
#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <ShapeGenerator.h>

namespace Engine
{
	LightManager::LightManager(int numStartingLights, Renderer* renderer)
	{
		initialized = false;
		currentLightID = 0;
		currentLightData = &pointLightsRawData[currentLightID];
		currentLight = &pointLights[currentLightID];
		shadowMapArray = UINT_MAX;

		up = glm::vec3(0.0f, 1.0f, 0.0f);

		activeLights = numStartingLights;
		this->renderer = renderer;

		mapWidth = 1024;
		mapHeight = 1024;

		projection = glm::perspective(60.0f, ((float)mapWidth/mapHeight), 1.0f, 200.0f);

		glm::mat4 bias(
			0.5f, 0, 0, 0.0f,
			0, 0.5f, 0, 0.0f,
			0, 0, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f
		);
		
		projectionBias = bias * projection;

		for(int i=0; i<activeLights; i++)
		{
			setCurrentLight(i);

			float startingScale = 12.0f;
			if(i==0)
			{
				currentLight->positionWorld = glm::vec4(startingScale, startingScale, startingScale, 1.0f); // needs better setup
				currentLight->diffuseColor = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f); // needs better setup
				//currentLightData->facing = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)); // needs better setup
			}
			else if(i==1)
			{
				currentLight->positionWorld = glm::vec4(startingScale, startingScale, -startingScale, 1.0f); // needs better setup
				currentLight->diffuseColor = glm::vec4(0.0f, 0.8f, 0.0f, 1.0f); // needs better setup
				//currentLightData->facing = glm::normalize(glm::vec3(-1.0f, -1.0f, 1.0f)); // needs better setup
			}
			else if(i==2)
			{
				currentLight->positionWorld = glm::vec4(-startingScale, startingScale, startingScale, 1.0f); // needs better setup
				currentLight->diffuseColor = glm::vec4(0.0f, 0.0f, 0.8f, 1.0f); // needs better setup
				//currentLightData->facing = glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f)); // needs better setup
			}
			else
			{
				currentLight->positionWorld = glm::vec4(-startingScale, startingScale, -startingScale, 1.0f); // needs better setup
				currentLight->diffuseColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f); // needs better setup
				//currentLightData->facing = glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f)); // needs better setup
			}
			currentLight->specularColor = currentLight->diffuseColor;
			updateMatrices();

			currentLight->active = true;
			currentLight->shadowMap = i;
		}
	}

	void LightManager::setCurrentLight(int targetLight)
	{
		if(targetLight < activeLights)
		{
			currentLightID = targetLight;
			currentLightData = &pointLightsRawData[currentLightID];
			currentLight = &pointLights[currentLightID];
		}
	}

	void LightManager::setPosition(glm::vec3& newPos)
	{
		glm::vec3 positionXYZ(currentLight->positionWorld);
		if( ! vec3Equal(newPos, positionXYZ))
		{
			currentLight->positionWorld = glm::vec4(newPos, 1.0f);
			updateMatrices();
		}
	}

	void LightManager::moveLight(glm::vec3& deltaPos)
	{
		currentLight->positionWorld += glm::vec4(deltaPos, 0.0f);
		updateMatrices();
	}

	void LightManager::setLightDirection(glm::vec3& newDir)
	{
		glm::vec3 normalizedDir(glm::normalize(newDir));
		if( ! vec3Equal(normalizedDir, currentLightData->facing))
		{
			currentLightData->facing = normalizedDir;
			updateMatrices();
		}
	}

	void LightManager::rotateLight(glm::mat4& rotation) // rotate facing, rebuild shadowMatrix and position
	{
		currentLightData->facing =  glm::vec3(rotation * glm::vec4(currentLightData->facing, 0.0f));
		updateMatrices();
	}

	void LightManager::setDiffuse(glm::vec4& newDiffuse)
	{
		currentLight->diffuseColor = newDiffuse;
	}

	void LightManager::setSpecular(glm::vec4& newSpecular)
	{
		currentLight->specularColor = newSpecular;
	}

	void LightManager::setActive(bool active)
	{
		currentLight->active = (active) ? 1 : 0;
	}

	void LightManager::writeAllData()
	{
		if(gpuData.uBuffer == UINT_MAX)
		{
			initialize();
		}

		gpuData.fullUpdate(pointLights);
	}

	void LightManager::initialize()
	{
		uint blockSize = sizeof(PointLightGLSL) * activeLights;
		bir = renderer->bufferManager.addData(blockSize, pointLights);
		gpuData.initialize(sizeof(PointLightGLSL), activeLights, bir.glBufferID, bir.offset);
		
		bindPoint = renderer->nextAvailableUniformBlockBindingPoint++;

		glBindBuffer(GL_UNIFORM_BUFFER, bir.glBufferID);

		// this will need to be called again if the number of active lights changes
		glBindBufferRange(GL_UNIFORM_BUFFER, bindPoint, bir.glBufferID, bir.offset, blockSize);

		// Texture stuff ...
		glActiveTexture(GL_TEXTURE31);
		float border[] = {1.0f, 0.0f, 0.0f, 0.0f};
		glGenTextures(1, &shadowMapArray);
		glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArray);

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT24, mapWidth, mapHeight, activeLights); //, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, border);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		
		initialized = true;
		
		/*
		Neumont::ShapeData cube = Neumont::ShapeGenerator::makeCube();
		lightShader = renderer->addProgram("Content_Shared\\LightRenderable.vsh", "Content_Shared\\LightRenderable.fsh");
		lightShader->enableViewProjection("VP");
		lightShader->enableMultiShadow("PointLights", bindPoint);
		uint lightGeo = renderer->addGeometry(cube.vertexBufferSize(), cube.indexBufferSize(), cube.numIndices, cube.verts, cube.indices, Neumont::Vertex::STRIDE, Engine::DrawStyle::TRIANGLES);

		uint formattedArr[] = {
			0, Neumont::Vertex::POSITION_OFFSET, 3,
			1, Neumont::Vertex::COLOR_OFFSET, 4
		};

		for(int i=0; i<activeLights; i++)
		{
			Renderable* r = renderer->addRenderable(glm::mat4(), lightGeo, lightShader, false);
			r->addUniform(UniformFormat::INT, "index", sizeof(int), &i);
			r->setShaderStreamedParameters(2, formattedArr);
		}
		*/
	}

	void LightManager::updateView()
	{
		//currentLight->view = glm::lookAt(currentLightData->positionWorld, currentLightData->positionWorld + currentLightData->facing, up);
		currentLightData->view = glm::lookAt(glm::vec3(currentLight->positionWorld), glm::vec3(0.0f), up);
	}

	void LightManager::updateShadowMatrix()
	{
		currentLight->shadowMatrix = projectionBias * currentLightData->view;
	}

	void LightManager::updateMatrices()
	{
		updateView();
		updateShadowMatrix();
	}

	void LightManager::updateAllMatrices()
	{
		int startingID = currentLightID;
		for(int i=0; i<activeLights; i++)
		{
			setCurrentLight(i);
			updateView();
			updateShadowMatrix();
		}
		setCurrentLight(startingID);
	}

	void LightManager::setAllColorsDefault()
	{
		int previousLightID = currentLightID;
		for(int i=0; i<activeLights; i++)
		{
			setCurrentLight(i);
			
			if(i==0)
				currentLight->diffuseColor = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f); // needs better setup
			else if(i==1)
				currentLight->diffuseColor = glm::vec4(0.0f, 0.8f, 0.0f, 1.0f); // needs better setup
			else if(i==2)
				currentLight->diffuseColor = glm::vec4(0.0f, 0.0f, 0.8f, 1.0f); // needs better setup
			else
				currentLight->diffuseColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f); // needs better setup
			
			currentLight->specularColor = currentLight->diffuseColor;
		}
		setCurrentLight(previousLightID);
	}

	void LightManager::setAllColorsWhite()
	{
		for(int i=0; i<activeLights; i++)
		{
			pointLights[i].diffuseColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
			pointLights[i].specularColor = pointLights[i].diffuseColor;
		}
	}

	glm::vec4 LightManager::getColorOfLight(int index)
	{
		return pointLights[index].diffuseColor;
	}

	glm::vec3 LightManager::getPositionOfLight(int index)
	{
		return glm::vec3(pointLights[index].positionWorld);
	}

	void LightManager::update()
	{
		if(GetAsyncKeyState(Qt::Key::Key_1))
		{
			setCurrentLight(0);
		}
		if(GetAsyncKeyState(Qt::Key::Key_2))
		{
			setCurrentLight(1);
		}
		if(GetAsyncKeyState(Qt::Key::Key_3))
		{
			setCurrentLight(2);
		}
		if(GetAsyncKeyState(Qt::Key::Key_4))
		{
			setCurrentLight(3);
		}

		float speed = 0.3f;

		if(GetAsyncKeyState(Qt::Key::Key_J))
			moveLight(glm::vec3(-speed, 0.0f, 0.0f));
		if(GetAsyncKeyState(Qt::Key::Key_L))
			moveLight(glm::vec3(speed, 0.0f, 0.0f));
		if(GetAsyncKeyState(Qt::Key::Key_I))
			moveLight(glm::vec3(0.0f, 0.0f, speed));
		if(GetAsyncKeyState(Qt::Key::Key_K))
			moveLight(glm::vec3(0.0f, 0.0f, -speed));
		if(GetAsyncKeyState(Qt::Key::Key_Y))
			moveLight(glm::vec3(0.0f, speed, 0.0f));
		if(GetAsyncKeyState(Qt::Key::Key_H))
			moveLight(glm::vec3(0.0f, -speed, 0.0f));
	}

	bool LightManager::vec3Equal(glm::vec3& a, glm::vec3& b)
	{
		return a.x == b.x
			&& a.y == b.y
			&& a.z == b.z;
	}

	bool LightManager::vec4Equal(glm::vec4& a, glm::vec4& b)
	{
		return a.x == b.x
			&& a.y == b.y
			&& a.z == b.z
			&& a.w == b.w;
	}
}
