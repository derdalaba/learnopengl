#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Position
{
public:
	Position(float x, float y, float z);
	Position();
	Position(glm::vec3 pos);
	glm::vec3 getPosition();
	void setPosition(float x, float y, float z);
	void setPosition(glm::vec3 pos);
	void setX(float x);
	void setY(float y);
	void setZ(float z);
	void rotate(float angle, glm::vec3 axis);
	void relativeSetPosition(float x, float y, float z);
	void relativeSetPosition(glm::vec3 pos);
	~Position();

private:
	glm::vec3 pos;
};

