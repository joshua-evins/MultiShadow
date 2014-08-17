#include "Renderer.h"
#include <Qt\qevent.h>
#include <Qt\qtextstream.h>
#include <Qt\qdebug.h>
#include <Qt\qfile.h>
#include <glm\gtx\transform.hpp>
#include <iostream>
#include <fstream>
#include <cstring>
#include "../DebugShapeHandler.h"

namespace Engine
{

	Renderer::Renderer() : lightManager(4, this)
	{
		paused = false;

		currentGeometriesIndex = 0;
		currentBufferedGeometriesIndex = 0;
		currentShaderProgramsIndex = 0;
		currentRenderablesIndex = 0;

		nextTextureIndex = 0;
		lightView = glm::mat4();
		lightProjection = glm::mat4();
		shadowShaderProgram = NULL;

		light.position = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		light.diffI = glm::vec4(1.0f);
		light.specI = glm::vec4(1.0f);

		lightRenderable = NULL;
		UseDiffuse = true;
		UseSpecular = true;
	}

	#pragma region Override Functions (GL init and paint)
	
	void Renderer::initializeGL()
	{
		glewInit();
		show();

		glClearColor(0.0f,0.0f,0.0f,0.0f);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		initializeSize();

		camera.initialize(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		projection = glm::perspective(60.0f, ((float)width()/height()), 0.1f, 200.0f);

		lightManager.initialize();

		glGenFramebuffers(1, &shadowFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(GL_TEXTURE_2D_ARRAY, lightManager.shadowMapArray);
		/*
		glFramebufferTexture3D(
			GL_FRAMEBUFFER, // target
			GL_DEPTH_ATTACHMENT, // attachment
			GL_TEXTURE_2D_ARRAY, // textarget
			lightManager.shadowMapArray, // texture
			0, // level
			1); // layer
		*/
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lightManager.shadowMapArray, 0, 1);

		uint drawBuffers[] = {GL_NONE};
		glDrawBuffers(1, drawBuffers);


		assert(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == 36053);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/*
		textures[0] = depthTex;
		nextTextureIndex++;

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &cubeMapTex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTex);
		
		const char* suffixes[] = {
			"posx", "negx", "posy", "negy", "posz", "negz"
		};
		
		uint targets[] = {
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		};

		for(int i=0; i < 6; i++)
		{
			std::string texName = std::string("Content_CubeMap\\CubeMap_") + suffixes[i] + ".png";
			std::cout << texName << std::endl;
			QImage img =  QGLWidget::convertToGLFormat(QImage(texName.c_str(), "png"));

			glTexImage2D(targets[i], 0, GL_RGBA, img.width(), img.height(),
				0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
		}

		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		*/
	}

	void Renderer::paintGL()
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		update();
	}

	#pragma endregion

	#pragma region Event Handlers

	void Renderer::keyPressEvent(QKeyEvent* )
	{
	}
	
	void Renderer::mousePressEvent(QMouseEvent* e)
	{
		lastX = e->x();
		lastY = e->y();
	}

	void Renderer::resizeEvent(QResizeEvent*)
	{
		initializeSize();
	}

	#pragma endregion

	void Renderer::compileShader(const char* fileName, GLuint shaderID)  
	{
		QFile input(fileName);
		
		if( ! input.open(QIODevice::ReadOnly | QIODevice::Text) )
		{
			qDebug() << "File failed to open: " << fileName;
			exit(1);
		}

		QTextStream stream(&input);
		QString shaderCodeText = stream.readAll();
		input.close();

		// old stuff
		//std::string temp = shaderCodeText.toStdString();
		//const char* shaderSource = temp.c_str();

		QByteArray ba = shaderCodeText.toLocal8Bit();
		const char* shaderSource = ba.data();

		glShaderSource(shaderID, 1, &shaderSource, NULL);
		glCompileShader(shaderID);

		GLint status;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
		if(status != GL_TRUE)
		{
			GLint infoLogLen;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLen);

			char* buffer = new char[infoLogLen];

			GLsizei BitBucket;

			glGetShaderInfoLog(shaderID, infoLogLen, &BitBucket, buffer);

			qDebug() << buffer;

			delete [] buffer;

			exit(1);
		}
	}

	#pragma region Add Component Functions

	uint Renderer::addTexture(const char* fileName, const char* imageExt)
	{
		QImage texture = QGLWidget::convertToGLFormat(QImage(fileName, imageExt));
		glActiveTexture(GL_TEXTURE0);
		
		glGenTextures(1, textures+nextTextureIndex);
		glBindTexture(GL_TEXTURE_2D, textures[nextTextureIndex]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width(),
		texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
		texture.bits());
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR);
		return nextTextureIndex++;
	}

	uint Renderer::addGeometry(uint vertexBufferSize, uint indexBufferSize, uint numIndices, void* verts, void* indices, uint stride, DrawStyle drawStyle)
	{
		assert( currentGeometriesIndex < GEOMETRIES_MAX_CAPACITY );

		BufferInsertionRecord verticesRecord = bufferManager.addData(vertexBufferSize, verts);
		BufferInsertionRecord indicesRecord = bufferManager.addData(indexBufferSize, indices);

		geometries[currentGeometriesIndex].initialize(verticesRecord.glBufferID, indicesRecord.glBufferID,
			verticesRecord.offset, indicesRecord.offset, numIndices, stride, drawStyle);

		return currentGeometriesIndex++;
	}

	BufferedGeometryInfo* Renderer::addBufferedGeometry(uint numVerts, uint shaderID, DrawStyle drawStyle)
	{
		assert(currentBufferedGeometriesIndex < BUFFEREDGEOMETRIES_MAX_CAPACITY);

		BufferedGeometryInfo& geo = bufferedGeometries[currentBufferedGeometriesIndex];

		geo.shaderID = shaderID;
		geo.numVerts = numVerts;
		geo.drawStyle = drawStyle;

		return bufferedGeometries + (currentBufferedGeometriesIndex++);
	}

	ShaderInfo* Renderer::addProgram(const char* vshFileName, const char* fshFileName)
	{
		assert( currentShaderProgramsIndex < SHADERPROGRAMS_MAX_CAPACITY);

		ShaderInfo& target = shaderPrograms[currentShaderProgramsIndex];

		target.vertexID = glCreateShader(GL_VERTEX_SHADER);
		target.fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

		compileShader(vshFileName, target.vertexID);
		compileShader(fshFileName, target.fragmentID);

		target.programID = glCreateProgram();

		glAttachShader(target.programID, target.vertexID);
		glAttachShader(target.programID, target.fragmentID);
		glLinkProgram(target.programID);

		target.numUniforms = 0;

		return shaderPrograms + currentShaderProgramsIndex++;
	}

	ShaderInfo* Renderer::addProgram(const char* vshFileName, const char* fshFileName, const char* gshFileName)
	{
		assert( currentShaderProgramsIndex < SHADERPROGRAMS_MAX_CAPACITY);

		ShaderInfo& target = shaderPrograms[currentShaderProgramsIndex];

		target.vertexID = glCreateShader(GL_VERTEX_SHADER);
		target.fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
		target.geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);

		compileShader(vshFileName, target.vertexID);
		compileShader(fshFileName, target.fragmentID);
		compileShader(gshFileName, target.geometryShaderID);

		target.programID = glCreateProgram();

		glAttachShader(target.programID, target.vertexID);
		glAttachShader(target.programID, target.fragmentID);
		glAttachShader(target.programID, target.geometryShaderID);
		glLinkProgram(target.programID);

		target.numUniforms = 0;

		return shaderPrograms + currentShaderProgramsIndex++;
	}

	Renderable* Renderer::addRenderable(glm::mat4& transform, uint geometryID, ShaderInfo* shaderProgramIndex, bool usesDepthTest, DrawLayer drawLayer, bool hasShadow)
	{
		assert( currentRenderablesIndex < RENDERABLES_MAX_CAPACITY);

		renderables[currentRenderablesIndex].initialize(transform, this, geometryID, shaderProgramIndex, true, usesDepthTest, drawLayer, hasShadow);

		return &(renderables[currentRenderablesIndex++]);
	}

	#pragma endregion

	void Renderer::update()
	{
	if(!paused)
	{
		if(lightRenderable != NULL)
			lightRenderable->transform = glm::translate(light.position.x, light.position.y, light.position.z);
		//if(UseDiffuse)
		lightManager.update();
		bool UseSpecular;
		shadowPasses();
		drawRenderables(camera.getViewMatrix(), projection);
		drawBufferedGeometries(camera.getViewMatrix(), projection);
		
		POINT globalPos;
		GetCursorPos(&globalPos);
		int globalx = globalPos.x;
		int globaly = globalPos.y;
		QPoint screenPos(mapFromGlobal(QPoint(globalx, globaly)));

		int screenX = screenPos.x();
		int screenY = screenPos.y();
		int moveH = screenX - lastX;
		int moveV = screenY - lastY;
		camera.update(moveH, moveV);
		lastX = screenX;
		lastY = screenY;
	}
	}

	void Renderer::shadowPasses()
	{
		if(shadowShaderProgram == NULL)
			return;
		glViewport(0, 0, lightManager.mapWidth, lightManager.mapHeight);
		
		uint realShader = shadowShaderProgram->programID;
		
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		
		uint shadowmapIndex = glGetSubroutineIndex(realShader, GL_FRAGMENT_SHADER, "recordDepth");
		uint renderingIndex = glGetSubroutineIndex(realShader, GL_FRAGMENT_SHADER, "shadeWithShadow");
		
		glUseProgram(realShader);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &shadowmapIndex); // use recordDepth

		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		
		lightManager.updateAllMatrices();
		lightManager.writeAllData();

		glBindBuffer(GL_UNIFORM_BUFFER, lightManager.bir.glBufferID);

		for(int i=0; i<lightManager.activeLights; i++)
		{
			PointLightGLSL& light = lightManager.pointLights[i];
			PointLight& lightData = lightManager.pointLightsRawData[i];
			uint shadowMapID = light.shadowMap;
			
			glm::mat4& theView = lightData.view;
			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lightManager.shadowMapArray, 0, i);
			glClear(GL_DEPTH_BUFFER_BIT);

			drawRenderables(theView, lightManager.projection, true); 
		}
		
		
		

		/*
		float* texRead = new float[1024*1024*4];
        glActiveTexture(GL_TEXTURE29);
		glBindTexture(GL_TEXTURE_2D_ARRAY, lightManager.shadowMapArray);
        
		glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, texRead);

        uchar* rgbaTex = new uchar[1024*1024*4];

        for(int w=0; w<1024; w++)
                for(int h=0; h<1024; h++)
                {
                    float f = texRead[1024*w + h] * 255.0f;
                    uchar* rgba = rgbaTex + 4*(1024*w + h);
                    uchar f2c = (uchar)f;
                    rgba[0] = f2c;
                    rgba[1] = f2c;
                    rgba[2] = f2c;
                    rgba[3] = 255;
                }

        QImage qimg(rgbaTex, 1024, 1024, QImage::Format_ARGB32);
        qimg.save("depthTex1.png");

        delete [] texRead;
        delete [] rgbaTex;
		*/

		
		

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		glDisable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		glUseProgram(realShader);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &renderingIndex); // use shadeWithShadow
		glViewport(0, 0, width(), height());

		//delete [] texRead;
		//delete [] rgbaTex;
	}

	void Renderer::drawRenderables(const glm::mat4& view, const glm::mat4& proj, bool shadowPass)
	{
		
		glm::mat4 viewPerspective(proj * view);
		
		for(uint currentLayer=0; currentLayer<DrawLayer::DRAWLAYER_COUNT; currentLayer++)
		{
			if(currentLayer & DrawLayer::LAYER_BLEND)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				glDisable(GL_BLEND);
			}

			for( uint i=0; i < currentRenderablesIndex; i++)
			{
				if(renderables[i].visible && renderables[i].drawLayer == currentLayer)
				{
					Renderable& renderable = renderables[i];
					
					if (shadowPass && !renderable.hasShadow)
						continue;

					if(renderables[i].useDepthTest)
						glEnable(GL_DEPTH_TEST);
					else
						glDisable(GL_DEPTH_TEST); 

					if(renderables[i].jamie & JamieFlags::JAMIE_SHADOWTEXTUREPLANE)
					{
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, depthTex);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
						if(!GetAsyncKeyState(Qt::Key::Key_Space))
							continue;
					}

					if(renderables[i].jamie & JamieFlags::JAMIE_SENDLIGHTBOOLS)
					{
						uint UL = glGetUniformLocation(renderable.shader->programID, "UseDiffuse");
						glUniform1i(UL, (UseDiffuse) ? 1 : 0);

						UL = glGetUniformLocation(renderable.shader->programID, "UseSpecular");
						glUniform1i(UL, (UseSpecular) ? 1 : 0);
					}

					ShaderInfo& shader = *renderables[i].shader;
					GeometryInfo& geometry = geometries[renderables[i].geometryID];
					UniformInfo* renderableUniforms = renderables[i].uniforms;
					UniformInfo* shaderUniforms = shader.uniforms;


					glUseProgram( shader.programID );
				
					glBindBuffer(GL_ARRAY_BUFFER, geometry.vBuffer);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.iBuffer);
				

					GLint uniformLocation;

					// apply standard uniforms
					applyStandardShaderUniforms(shader, renderables[i].transform, view, proj, renderables[i].numTextures);
					

					// apply shader uniforms
					for(uint j=0; j<shader.numUniforms; j++)
					{
						uniformLocation = glGetUniformLocation( shader.programID, shaderUniforms[j].name);					
						applyUniform(uniformLocation, shaderUniforms+j, view, viewPerspective);
					}


					//apply renderable-specific uniforms
					for(uint j=0; j<renderable.numUniforms; j++)
					{
						uniformLocation = glGetUniformLocation( shader.programID, renderableUniforms[j].name);						
						applyUniform(uniformLocation, renderableUniforms+j, view, viewPerspective);
					}


					for(uint j=0; j<renderable.numVAttribs;j++)
					{
						VAttribInfo& attrib = renderable.vAttribs[j];
						glEnableVertexAttribArray(attrib.location);
						glVertexAttribPointer(attrib.location, attrib.size, attrib.element, GL_FALSE, geometry.stride, BUFFER_OFFSET(geometry.vOffset + attrib.offset));
					}

					for(uint k=0; k<renderables[i].numTextures; k++)
					{
						TextureInfo& tex = renderables[i].textureInfos[k];

						glActiveTexture(GL_TEXTURE0 + k);
						glBindTexture(GL_TEXTURE_2D, textures[tex.textureID]);
						glUniform1i(tex.uniformLoc, k);
					}
					
					if(renderables[i].jamie & JamieFlags::JAMIE_SIMPLENORMALMAPCHECKBOX)
					{
						if(!useFancyNormals)
						{
							uint UL = glGetUniformLocation(renderable.shader->programID, "NormalMap");
							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, textures[simpleNormalMap]);
							glActiveTexture(GL_TEXTURE1);
							glBindTexture(GL_TEXTURE_2D, textures[simpleNormalMap]);
							glUniform1i(UL, 0);
						}
					}
					
					for(uint k=0; k<renderables[i].numUniformBlocks; k++)
					{
						UniformBlockInfo& ubi = renderables[i].uniformBlocks[k];
						glBindBuffer(GL_UNIFORM_BUFFER, ubi.uBuffer);
						glUniformBlockBinding(shader.programID, ubi.uniformLocation, ubi.bindPoint);
					}
					
					for(uint k=0; k<shader.numUniformBlocks; k++)
					{
						UniformBlockInfo& ubi = shader.uniformBlocks[k];
						glBindBuffer(GL_UNIFORM_BUFFER, ubi.uBuffer);
						glUniformBlockBinding(shader.programID, ubi.uniformLocation, ubi.bindPoint);
					}

					if(geometry.drawStyle == DrawStyle::LINES)
						glDrawElements(GL_LINES, geometry.numIndices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(geometry.iOffset));
					else if(geometry.drawStyle == DrawStyle::TRIANGLES)
						glDrawElements(GL_TRIANGLES, geometry.numIndices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(geometry.iOffset));
					else if(geometry.drawStyle == DrawStyle::POINT_ARRAY)
						glDrawArrays(GL_POINTS, 0, geometry.numIndices);

				}
			}
		}
	}

	bool Renderer::applyUniform(uint uniformLocation, UniformInfo* info, const glm::mat4& view, const glm::mat4& viewPerspective)
	{
		if(uniformLocation >= 0)
		{
			switch(info->format)
			{
				case UniformFormat::GLMVEC2:
					glUniform2fv(uniformLocation, 1, reinterpret_cast<float*>(info->data));
					break;
				case UniformFormat::GLMVEC3:
					glUniform3fv(uniformLocation, 1, reinterpret_cast<float*>(info->data));
					break;
				case UniformFormat::GLMVEC4:
					glUniform4fv(uniformLocation, 1, reinterpret_cast<float*>(info->data));
					break;
				case UniformFormat::GLMVEC4_AFFECTEDBYVIEW:
					glUniform4fv(uniformLocation, 1, &(view * *reinterpret_cast<glm::vec4*>(info->data))[0]);
					break;
				case UniformFormat::GLMMAT3:
					glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, reinterpret_cast<float*>(info->data));
					break;
				case UniformFormat::GLMMAT4:
					glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, reinterpret_cast<float*>(info->data));
					break;
				case UniformFormat::FLOAT:
					glUniform1fv(uniformLocation, 1, reinterpret_cast<float*>(info->data));
					break;
				case UniformFormat::INT:
					glUniform1iv(uniformLocation, 1, reinterpret_cast<int*>(info->data));
					break;
				case UniformFormat::GLMMAT4_AFFECTEDBYVIEWPERSPECTIVE:
					glUniformMatrix4fv( uniformLocation, 1, GL_FALSE, &(viewPerspective * *reinterpret_cast<glm::mat4*>(info->data))[0][0] );
					break;
				case UniformFormat::GLMMAT4_AFFECTEDBYVIEW:
					glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &(view * *reinterpret_cast<glm::mat4*>(info->data))[0][0]);
					break;
				case UniformFormat::NORMALMAT_FROM_MODELMAT:
					glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, &(glm::mat3(view * *reinterpret_cast<glm::mat4*>(info->data))[0][0]));
					break;
				default:
					qDebug() << "Invalid UniformFormat";
			}
		}
		else
		{
			std::cout << info->name << " failed to send." << std::endl;
		}
		
		return uniformLocation >= 0;
	}

	void Renderer::applyStandardShaderUniforms(ShaderInfo& shader, glm::mat4& transform, const glm::mat4& view, const glm::mat4& proj, int firstOpenTextureSlot)
	{
		if(shader.sendMVP)
			glUniformMatrix4fv( shader.mvpUL, 1, GL_FALSE, &(proj * view * transform)[0][0] );

		if(shader.sendModelMat)
			glUniformMatrix4fv( shader.modelMatUL, 1, GL_FALSE, &(transform)[0][0] );

		if(shader.sendModelView)
			glUniformMatrix4fv( shader.modelViewUL, 1, GL_FALSE, &(view * transform)[0][0] );

		if(shader.sendNormalMat)
			glUniformMatrix3fv( shader.normalMatUL, 1, GL_FALSE, &(glm::mat3(view*transform))[0][0] );
		
		if(shader.sendLight)
		{
			glUniform4fv( shader.lightPositionUL, 1, &(view * light.position)[0] );
			if(shader.lightDiffuseIntensityUL >= 0)
				glUniform4fv( shader.lightDiffuseIntensityUL, 1, &light.diffI[0] );
			if(shader.lightSpecularIntensityUL >= 0)
				glUniform4fv( shader.lightSpecularIntensityUL, 1, &light.specI[0] );
		}

		if(shader.sendViewMat)
			glUniformMatrix4fv( shader.viewMatUL, 1, GL_FALSE, &view[0][0] );

		if(shader.sendShadow)
		{
			lightView = glm::mat4(glm::lookAt(glm::vec3(light.position), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
			glUniformMatrix4fv( shader.shadowMatUL, 1, GL_FALSE, &(bias * lightProjection * lightView * transform)[0][0] );

			glActiveTexture(GL_TEXTURE0 + firstOpenTextureSlot);
			glBindTexture(GL_TEXTURE_2D, depthTex);
			glUniform1i(shader.shadowMapUL, firstOpenTextureSlot);
		}

		if(shader.sendCubeMap)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTex);
			glUniform1i(shader.cubeMapUL, 0);
		}

		if(shader.sendCameraPosWorld)
			glUniform3fv(shader.cameraPosWorldUL, 1, &camera.position.x);

		if(shader.sendViewProjection)
			glUniform4fv(shader.viewProjectionUL, 1, &(proj * view)[0][0]);
	}

	// old function: may no longer work properly
	void Renderer::drawBufferedGeometries(const glm::mat4& view, const glm::mat4& projection)
	{
		glm::mat4 viewPerspective(projection * view);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,);

		for(int i=0; i<currentBufferedGeometriesIndex; i++)
		{
			BufferedGeometryInfo& geo = bufferedGeometries[i];
			glUseProgram(geo.shaderID);
			for(int j=0; j<geo.numAttribs; j++)
			{
				glEnableVertexAttribArray(j);
				AttribBufferInfo& attrib = geo.attribs[j];
				//data for an attribBuffer: buffer, size, type, stride
				// data for a BGI: numAttribs, attribs, drawStyle, numVerts
				glBindBuffer(GL_ARRAY_BUFFER, attrib.buffer);
				
				glVertexAttribPointer(j, attrib.size, attrib.glType, GL_FALSE, attrib.stride, 0);
			}

			uint uniformLocation = -1;
			UniformInfo* uniforms = geo.uniforms;
			for(uint j=0; j<geo.numUniforms; j++)
			{
				uniformLocation = glGetUniformLocation( geo.shaderID, uniforms[j].name);
				if(uniformLocation >= 0)
				{
					switch(uniforms[j].format)
					{
						case UniformFormat::GLMVEC3:
							glUniform3fv(uniformLocation, 1, reinterpret_cast<float*>(uniforms[j].data));
							break;
						case UniformFormat::GLMVEC4:
							glUniform4fv(uniformLocation, 1, reinterpret_cast<float*>(uniforms[j].data));
							break;
						case UniformFormat::GLMMAT3:
							glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, reinterpret_cast<float*>(uniforms[j].data));
							break;
						case UniformFormat::GLMMAT4:
							glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, reinterpret_cast<float*>(uniforms[j].data));
							break;
						case UniformFormat::FLOAT:
							glUniform1fv(uniformLocation, 1, reinterpret_cast<float*>(uniforms[j].data));
							break;
						case UniformFormat::INT:
							glUniform1iv(uniformLocation, 1, reinterpret_cast<int*>(uniforms[j].data));
							break;
						case UniformFormat::GLMMAT4_AFFECTEDBYVIEWPERSPECTIVE:
							glUniformMatrix4fv( uniformLocation, 1, GL_FALSE, &(viewPerspective * *reinterpret_cast<glm::mat4*>(uniforms[j].data))[0][0] );
							break;
						case UniformFormat::GLMMAT4_AFFECTEDBYVIEW:
							glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &(view * *reinterpret_cast<glm::mat4*>(uniforms[j].data))[0][0]);
							break;
						default:
							qDebug() << "Invalid UniformFormat";
					}
				}
			}

			glPointSize(pointSize);
			glDrawArrays(GL_POINTS, 0, geo.numVerts);
		}
	}

	void Renderer::initializeSize()
	{
		glViewport(0,0,width(), height());
		projection = glm::perspective(60.0f, ((float)width()/height()),
			0.1f, 200.0f);
		
	}

	#pragma region Debug
		#ifdef ENGINE_DEBUG
			void Renderer::debug_setCamera(Camera* cameraIn)
			{
				debugCamera = cameraIn;
			}

			GeometryInfo* Renderer::debug_getGeometryInfoAtIndex(uint geometryID)
			{
				return &geometries[geometryID];
			}

			Renderable* Renderer::debug_getRenderables()
			{
				return renderables;
			}
		#endif
	#pragma endregion

}