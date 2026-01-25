#pragma once
#include "Model.h"
#include <shader_m.h>
#include <glm/glm.hpp>

struct BoundingBox
{
	glm::vec3 min;
	glm::vec3 max;
	BoundingBox(glm::vec3 min, glm::vec3 max) : min(min), max(max){}
	BoundingBox() : min(glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX)), max(glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX)) {}
};

class RenderObject
{
public:
	float texScale = 1.0f;
	glm::mat4 modelMatrix;
	glm::vec3 pos;
	glm::vec3 scale;
	glm::vec3 rotation;
	float gamma;
	Model& model;
	Shader& shader;
	BoundingBox boundingBox = BoundingBox(glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX), glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX));
	bool drawBoundingBox = false; // Flag to control whether to draw the bounding box

    // Korrigierter Konstruktor für RenderObject
    RenderObject(Model& model, Shader& shader, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), float gamma = 1.0f)
        : pos(pos), scale(scale), rotation(rotation), model(model), shader(shader), gamma(gamma)
	{
		initModelMatrix();
	}
	void Draw(glm::mat4 projection, glm::mat4 view, glm::vec3 viewPos) //generate visual output
	{
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setVec3("viewPos", viewPos);
		shader.setFloat("gamma", gamma);
		shader.setFloat("texScale", texScale);
		shader.setMat4("model", modelMatrix);
		
		model.Draw(shader);
	}
	void Draw()
	{
		shader.setMat4("model", modelMatrix);
		model.Draw(shader);
	}
	void initModelMatrix()
	{
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, pos);
		modelMatrix = glm::scale(modelMatrix, scale);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	void setTexScale(float scale)
	{
		texScale = scale;
	}
	void setPosition(glm::vec3 position)
	{
		// Update the bounding box when the position changes
		glm::vec3 offset = position - pos; // Calculate the offset from the previous position
		boundingBox.min += offset;
		boundingBox.max += offset; // Update both min and max to maintain the bounding box size
		this->pos = position;
		initModelMatrix();
	}
	void setScale(glm::vec3 scale)
	{
		this->scale = scale;
		// Update the bounding box based on the new scale
		genBoundingBox();
		boundingBox.min = boundingBox.min * scale;
		boundingBox.max = boundingBox.max * scale;
		initModelMatrix();
	}
	void setRotation(glm::vec3 rotation)
	{
		this->rotation = rotation;
		initModelMatrix();
	}
	void setGamma(float gamma)
	{
		this->gamma = gamma;
		
	}
	void genBoundingBox()
	{
		for (Mesh mesh : model.meshes)
		{
			for (Vertex& vertex : mesh.vertices)
			{
				if (vertex.Position.x < boundingBox.min.x) boundingBox.min.x = vertex.Position.x;
				if (vertex.Position.y < boundingBox.min.y) boundingBox.min.y = vertex.Position.y;
				if (vertex.Position.z < boundingBox.min.z) boundingBox.min.z = vertex.Position.z;
				if (vertex.Position.x > boundingBox.max.x) boundingBox.max.x = vertex.Position.x;
				if (vertex.Position.y > boundingBox.max.y) boundingBox.max.y = vertex.Position.y;
				if (vertex.Position.z > boundingBox.max.z) boundingBox.max.z = vertex.Position.z;
			}
		}
		boundingBox.min *= scale;
		boundingBox.max *= scale;
		boundingBox.min += pos;
		boundingBox.max += pos;
	}
	bool isPointInside(glm::vec3 point)
	{
		return (point.x >= boundingBox.min.x && point.x <= boundingBox.max.x &&
			point.y >= boundingBox.min.y && point.y <= boundingBox.max.y &&
			point.z >= boundingBox.min.z && point.z <= boundingBox.max.z);
	}
	bool boundingBoxCollision(RenderObject& other)
	{
		return (boundingBox.min.x <= other.boundingBox.max.x && boundingBox.max.x >= other.boundingBox.min.x &&
			boundingBox.min.y <= other.boundingBox.max.y && boundingBox.max.y >= other.boundingBox.min.y &&
			boundingBox.min.z <= other.boundingBox.max.z && boundingBox.max.z >= other.boundingBox.min.z);
	}
private:
};
