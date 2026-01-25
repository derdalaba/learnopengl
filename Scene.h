#pragma once
#include "Light.h"

class Scene
{
public:
	std::vector<Shader*> shaders;
	std::vector<RenderObject> objects;
	std::vector<Light> lights;
	std::vector<Camera*> cameras;
	SkyBox* skybox;
	Camera* currentCamera;
	
	Scene(std::vector<Shader*> shaders, std::vector<RenderObject> objects, std::vector<Camera*> cameras, SkyBox* skyBox, unsigned int screenWidth, unsigned int screenHight)
		: shaders{ shaders }, objects{ objects }, cameras{ cameras }, skybox(skyBox),
    SCR_WIDTH{ screenWidth }, SCR_HEIGHT{ screenHight }, projection(1.0f)
	{
		currentCamera = cameras.empty() ? nullptr : cameras[0];
		if (currentCamera)
			this->projection = glm::perspective(glm::radians(currentCamera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);
	}
	
	void use()
	{
		setupLights();
	}
	
	void Draw() 
	{
		if (!currentCamera) return;
		glm::mat4 view = currentCamera->GetViewMatrix();
		glm::vec3 viewPos = currentCamera->Position;
		Shader* lastShader = nullptr;
		for (RenderObject& obj : objects)
		{
			Shader* currentShader = &obj.shader;
			if (lastShader and currentShader->ID == lastShader->ID) {
				obj.Draw();
			}
			else {
				currentShader->use();
				obj.Draw(projection, view, currentCamera->Position);
				lastShader = currentShader;
			}
		}
		skybox->Draw(projection, view);
	}
	~Scene()
	{
	}
	void updateScreenSize(int width, int height)
	{
		SCR_WIDTH = width;
		SCR_HEIGHT = height;
		if (currentCamera)
			projection = glm::perspective(glm::radians(currentCamera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);
	}
private:
	unsigned int SCR_WIDTH;
	unsigned int SCR_HEIGHT;
	glm::mat4 projection = glm::mat4(1.0f);

	void setupLights()
	{

	}
};
