#pragma once

#include <glm/glm.hpp>
#include <camera.h>


class Player
{
public:
	Player(float x, float y, float z);
	Player();
	Player(Camera camera);
	Player(glm::vec3 pos);
	glm::vec3 getPosition();
	void setPosition(float x, float y, float z);
	void setPosition(glm::vec3 pos);
	void processKeyboard(Camera_Movement direction, float timeDelta);
	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void processMouseScroll(float yoffset);
	Camera getCamera() { return camera; }
	~Player();
private:
	Camera camera;
};

