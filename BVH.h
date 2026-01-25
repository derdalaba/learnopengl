#pragma once
#include "RenderObject.h"
#include <vector>
#include <glm/glm.hpp>
#include "BVH_Node.h"

class BVH
{
public:
	RenderObject& model; // Generate bvh for each mesh in the model
	std::vector<BVH_Node> roots; // Root node of the BVH tree

	BVH(RenderObject& model) : model(model)
	{
		for (Mesh& mesh : model.model.meshes)
		{
			roots.push_back(BVH_Node(mesh));
			// Further initialization can be done here if needed
		}
	}
	void updateBVH()
	{
		// Implementation for updating the BVH based on the model's bounding boxes
		// This would typically involve calculating the bounding boxes of the model's meshes
		// and organizing them into a hierarchical structure.
		
	}
	~BVH();
	
	BoundingBox getBoundingBox()
	{
		return model.boundingBox;
	}
	vector<Vertex> getVertices(glm::vec3 point, float radius)
	{

	}

private:
	
};

BVH::~BVH()
{
}