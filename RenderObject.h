#include "Model.h"
#include <shader_m.h>
#include <glm/glm.hpp>

class RenderObject
{
public:
	float texScale = 1.0f;
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
	Model* model;
	Shader* shader;

	RenderObject(Model* model, Shader* shader)
	{
		this->model = model;
		this->shader = shader;
	}
	virtual void Draw() //generate visual output
	{
	}
	void Pick(Shader pickShader, int id) //generate uniformally colord meshes for identification/object picking
	{
		pickShader.setInt("id", id);
		model->Draw(pickShader);
	}
private:
	
};
