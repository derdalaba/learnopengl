#include "Model.h"
#include <shader_m.h>

class RenderObject
{
public:
	RenderObject renderObject(Model model, Shader shader)
	{
		this->model = model;
		this->shader = shader;
	}
	void Draw() //generate visual output
	{
		model.Draw(shader);
	}
	void Pick(Shader pickShader, int id) //generate uniformally colord meshes for identification/object picking
	{
		pickShader.setInt("id", id);
		model.Draw(pickShader);
	}
private:
	Model model;
	Shader shader;
};
