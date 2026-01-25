#pragma once
#include <glm/glm.hpp>

enum class LightType
{
	Point, Directional, Spot
};

class Light
{
	public:
	glm::vec3 position;
	glm::vec3 color;
	float intensity;
	LightType type;
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;

	Light(glm::vec3 position, glm::vec3 color, float intensity, LightType type)
		: position(position), color(color), intensity(intensity), type(type)
	{
	}
	Light(glm::vec3 position, glm::vec3 direction, glm::vec3 color, float intensity, float cutOff, float outerCutOff)
		: position(position), color(color), intensity(intensity), direction(direction), cutOff(cutOff), outerCutOff(outerCutOff)
	{
		type = LightType::Spot;
	}
};