#include "position.h"

Position::Position(float x, float y, float z)
{
	pos = glm::vec3(x, y, z);
}
Position::Position()
{
	pos = glm::vec3(0.0f, 0.0f, 0.0f);
}
Position::Position(glm::vec3 pos)
{
	this->pos = pos;
}
void Position::setPosition(float x, float y, float z)
{
	pos = glm::vec3(x, y, z);
}
void Position::setPosition(glm::vec3 pos)
{
	this->pos = pos;
}
void Position::setX(float x)
{
	pos.x = x;
}
void Position::setY(float y)
{
	pos.y = y;
}
void Position::setZ(float z)
{
	pos.z = z;
}
void Position::relativeSetPosition(float x, float y, float z)
{
	pos += glm::vec3(x, y, z);
}
void Position::relativeSetPosition(glm::vec3 pos)
{
	this->pos += pos;
}
void Position::rotate(float angle, glm::vec3 axis)
{
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
	glm::vec4 newPos = rotationMatrix * glm::vec4(pos, 1.0f);
	pos = glm::vec3(newPos);
}
glm::vec3 Position::getPosition()
{
	return pos;
}
Position::~Position()
{
}