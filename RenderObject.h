#include "Model.h"
#include <shader_m.h>
#include <glm/glm.hpp>

class RenderObject
{
public:
	float texScale = 1.0f;
	glm::vec3 pos;
	glm::vec3 scale;
	glm::vec3 rotation;
	float gamma;
	Model& model;
	Shader& shader;

    // Korrigierter Konstruktor für RenderObject
    RenderObject(Model& model, Shader& shader, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), float gamma = 1.0f)
        : pos(pos), scale(scale), rotation(rotation), model(model), shader(shader), gamma(gamma)
	{}
	void Draw(glm::mat4 projection, glm::mat4 view) //generate visual output
	{
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setFloat("gamma", gamma);
		shader.setFloat("texScale", texScale);
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, pos);
		modelMatrix = glm::scale(modelMatrix, scale);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		shader.setMat4("model", modelMatrix);
		
		model.Draw(shader);
	}
	void setTexScale(float scale)
	{
		texScale = scale;
	}
	void setPosition(glm::vec3 position)
	{
		this->pos = position;
	}
	void setScale(glm::vec3 scale)
	{
		this->scale = scale;
	}
	void setRotation(glm::vec3 rotation)
	{
		this->rotation = rotation;
	}
	void setGamma(float gamma)
	{
		this->gamma = gamma;
		
	}
private:
};
