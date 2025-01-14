#pragma once
#include <glm/glm.hpp>
#include <GL/gl3w.h>
#include <string>
#include <vector>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stbimage/stb_image.h>

//NOW USING ASSIMP


struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};


struct Texture
{
	GLuint id;
	std::string type;
	std::string path;

};



namespace ModelLoader
{

	uint32_t textureFromFile(const char*, const std::string, bool);

	class Mesh
	{
	public:
		std::vector<Vertex> verts;
		std::vector<uint32_t> indices;
		std::vector<Texture> textures;

		Mesh(std::vector<Vertex> v, std::vector<uint32_t> idx, std::vector<Texture> tex)
			: verts(v), indices(idx), textures(tex)
		{
			setupMesh();
		}

	private:
		GLuint vao, vbo, ibo;

		void setupMesh();
	};

	class Model
	{
	public:
		Model(char* path)
		{
			loadModel(path);
		}
	private:
		std::vector<Mesh> meshes;
		std::string dir;
		std::vector<Texture> loadedTextures;

		void loadModel(std::string);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadTextures(
			aiMaterial*, aiTextureType, std::string);


	};
};



