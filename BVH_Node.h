#pragma once
#include <optional>
#include <glm/glm.hpp>
#include <vector>
#include "RenderObject.h"

class BVH_Node
{
public:
	BoundingBox boundingBox; // Bounding box of this node
	std::vector<BVH_Node> children; // Children nodes of this BVH node
	std::vector<aiFace> faces; // Vertices associated with this node, if any
	Mesh& mesh; // Reference to the model associated with this node
	BVH_Node* parent = nullptr; // Pointer to the parent node, if any
	BVH_Node(Mesh& mesh) : mesh(mesh)
	{
		boundingBox = genBoundingBox();
		// Initialize children if necessary
		// This could be based on the number of meshes in the model or other criteria
	}
	BVH_Node(BVH_Node* parent)
		: mesh(parent->mesh)
	{
		std::optional<std::vector<BVH_Node>> newChildren = parent->split();
		if (newChildren.has_value())
		{
			children = newChildren.value();
			for (BVH_Node& child : children)
			{
				child.parent = this;
			}
		} else {
			faces = parent->faces;
		}
	}
	std::optional<std::vector<BVH_Node>> split()
	{
		// Logic to split the current node into multiple children based on some criteria
		// This could involve partitioning the vertices or bounding box
		if (parent->faces.empty() || faces.size() <= 1)
			return std::nullopt; // No vertices to split. Not enough vertices to split

		std::optional<std::vector<BVH_Node>> newChildren;
		Mesh& leftMesh = mesh; // Placeholder
		Mesh& rightMesh = mesh; // Placeholder
		axis longestAxis = getLongestAxis();
		for (aiFace face : faces) {
			for (unsigned int index = 0; index < face.mNumIndices; index++) {
				switch (longestAxis)
				{
					case axis::X:
						if (mesh.vertices[face.mIndices[index]].Position.x < (boundingBox.min.x + boundingBox.max.x) / 2.0f) {
							// Assign to left child
							leftMesh.vertices.push_back(mesh.vertices[face.mIndices[index]]);
						} else {
							// Assign to right child
							rightMesh.vertices.push_back(mesh.vertices[face.mIndices[index]]);
						}
						break;
					case axis::Y:
						if (mesh.vertices[face.mIndices[index]].Position.y < (boundingBox.min.y + boundingBox.max.y) / 2.0f) {
							// Assign to left child
							leftMesh.vertices.push_back(mesh.vertices[face.mIndices[index]]);
						}
						else {
							// Assign to right child
							rightMesh.vertices.push_back(mesh.vertices[face.mIndices[index]]);
						}
						break;
					case axis::Z:
						if (mesh.vertices[face.mIndices[index]].Position.z < (boundingBox.min.z + boundingBox.max.z) / 2.0f) {
							// Assign to left child
							leftMesh.vertices.push_back(mesh.vertices[face.mIndices[index]]);
						}
						else {
							// Assign to right child
							rightMesh.vertices.push_back(mesh.vertices[face.mIndices[index]]);
						}
						break;
				}
				
			}
		}
		newChildren = std::vector<BVH_Node>{ BVH_Node(leftMesh), BVH_Node(rightMesh) };
		
		return newChildren;
	}
private:
	enum class axis
	{
		X,
		Y,
		Z
	};
	axis getLongestAxis()
	{
		glm::vec3 size = boundingBox.max - boundingBox.min;
		if (size.x >= size.y && size.x >= size.z)
			return axis::X;
		else if (size.y >= size.x && size.y >= size.z)
			return axis::Y;
		else
			return axis::Z;
	}
	BoundingBox genBoundingBox()
	{
		BoundingBox box;
		for (aiFace face : faces)
		{
			for (int idx = 0; idx < face.mNumIndices; idx++)
			{
				Vertex vert = mesh.vertices[face.mIndices[idx]];
				// Calculate the bounding box based on the vertices
				box.min = glm::min(box.min, vert.Position);
				box.max = glm::max(box.max, vert.Position);
			}
		}
		return box;
	}
};
