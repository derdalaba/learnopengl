#pragma once
#include <vector>
#include "RenderObject.h"

class Scene
{
public:
	std::vector<RenderObject> renderQue;
	Scene()
	{
	}
	
	void Draw(glm::mat4 view, glm::mat4 projection) 
	{
		for (RenderObject obj : renderQue) 
		{
			obj.Draw(view, projection);
		}
	}
	~Scene()
	{
	}
private:
	
};



