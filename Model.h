#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include <shader_m.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <chrono>

using namespace std;

struct Pill
{
    glm::vec3 position;
    float radius;
    float height;

    bool intersect(Face face)
    {

    }
};

class Model
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;
    Pill pill;

    Model(float sideLength, unsigned int divisions, std::string texturePath) 
    {
        chrono::steady_clock::time_point begin = chrono::steady_clock::now();
		// Generate a plane model procedurally
        vector<Vertex> vertices = generatePlaneVertices(sideLength, divisions);
        vector<unsigned int> indices = generatePlaneIndices(vertices);
		Texture texture;
		texture.id = TextureFromFile(texturePath.c_str(), "");
		texture.type = "texture_height";
		texture.path = texturePath;
		textures_loaded.push_back(texture);
		meshes.push_back(Mesh(vertices, indices, textures_loaded));
		chrono::steady_clock::time_point end = chrono::steady_clock::now();
		cout << "Procedural plane model generated in milliseconds: " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms" << endl;
    }

    // constructor, expects a filepath to a 3D model.
    Model(string const& path)
    {
		chrono::steady_clock::time_point begin = chrono::steady_clock::now();
        loadModel(path);
        for (Texture tex : textures_loaded) {
            std::cout << tex.type << " loaded at path: " << tex.path << std::endl;
        }
		chrono::steady_clock::time_point end = chrono::steady_clock::now();
		cout << "Model loaded in milliseconds: " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms" << endl;
    }
    // draws the model, and thus all its meshes
    Model(string const& path, Pill pill)
    {
        loadModel(path);
		this->pill = pill;
	}

    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
    unsigned int TextureFromFile(const char* path, const string& directory);
private:
    vector<Vertex> generatePlaneVertices(float sideLength, unsigned int divisions)
    {
        double unit = (sideLength / divisions);
        vector<Vertex> vertices;
        float halfSide = sideLength / 2.0f;
        for (unsigned int z = 0; z <= divisions; z++)
        {
            for (unsigned int x = 0; x <= divisions; x++)
            {
                Vertex vertex;
                vertex.Position = glm::vec3(unit * x - halfSide, 0.0f, unit * z - halfSide);
                vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
                vertex.TexCoords = glm::vec2(static_cast<float>(x) / divisions, static_cast<float>(z) / divisions);
                vertex.Tangent = glm::vec3(1.0f, 0.0f, 0.0f);
                vertex.Bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
                vertices.push_back(vertex);
            }
        }
		return vertices;
    }

    vector<unsigned int> generatePlaneIndices(const vector<Vertex>& vertices)
    {
        vector<unsigned int> indices;
        unsigned int sideLength = static_cast<unsigned int>(sqrt(vertices.size())) - 1;
        for (unsigned int z = 0; z < sideLength; z++)
        {
            for (unsigned int x = 0; x < sideLength; x++)
            {
                unsigned int topLeft = z * (sideLength + 1) + x;
                unsigned int topRight = topLeft + 1;
                unsigned int bottomLeft = (z + 1) * (sideLength + 1) + x;
                unsigned int bottomRight = bottomLeft + 1;
                // First triangle
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);
                // Second triangle
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
		}
        return indices;
	}
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_ImproveCacheLocality);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('\\'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 5. ambient maps
		std::vector<Texture> aoMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ao");
		textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
		// 6. roughness maps
		std::vector<Texture> roughnessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "texture_roughness");
		textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
		// 7. height maps
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_DISPLACEMENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};



#endif