#include "Player.h"

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

Player::Player(float x, float y, float z)
{
	camera = Camera(glm::vec3(x, y, z));
}
Player::Player()
{
	camera = Camera();
}
Player::Player(Camera camera)
{
	this->camera = camera;
}
Player::Player(glm::vec3 pos)
{
	camera = Camera(pos);
}
void Player::setPosition(float x, float y, float z)
{
	camera.Position = glm::vec3(x, y, z);
}
void Player::setPosition(glm::vec3 pos)
{
	camera.Position = pos;
}

void Player::processKeyboard(Camera_Movement direction, float deltaTime)
{
	camera.ProcessKeyboard(direction, deltaTime);
}
void Player::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	camera.ProcessMouseMovement(xoffset, yoffset, constrainPitch);
}
void Player::processMouseScroll(float yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
glm::vec3 Player::getPosition()
{
	return camera.Position;
}
Player::~Player()
{
}