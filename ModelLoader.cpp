#include "ModelLoader.h"


namespace ModelLoader
{
	uint32_t textureFromFile(const char* path, const std::string dir, bool gamma = false)
	{
		std::string fn = std::string(path);
		fn = dir + '/' + fn;

		GLuint id;
		glGenTextures(1, &id);

		int w, h, c;
		unsigned char* data = stbi_load(fn.c_str(), &w, &h, &c, 0);

		if (data)
		{
			GLenum fmt;
			if (c == 1) fmt = GL_RED;
			if (c == 3) fmt = GL_RGB;
			if (c == 4) fmt = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "failed to load image at: " << path << std::endl;
			stbi_image_free(data);
		}
		return id;
	}

	void Mesh::setupMesh()
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ibo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		
		glBindVertexArray(0);

	}

	void Model::loadModel(std::string path)
	{
		Assimp::Importer importer;
		//Flags JoinIdenticalVertices will remove the need to manual optimise our meshes using a map
		//
		const aiScene* scene = importer.ReadFile(path,
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "Assimp Error: " << importer.GetErrorString() << std::endl;
			return;
		}
		dir = path.substr(0, path.find_last_of('/'));
		processNode(scene->mRootNode, scene);
	}

	void Model::processNode(aiNode* node, const aiScene* scene)
	{
		for (size_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		for (size_t i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> verts;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		for (size_t i = 0; mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 v;
			v.x = mesh->mVertices[i].x;
			v.y = mesh->mVertices[i].y;
			v.z = mesh->mVertices[i].z;
			vertex.position = v;

			v.x = mesh->mNormals[i].x;
			v.y = mesh->mNormals[i].y;
			v.z = mesh->mNormals[i].z;
			vertex.position = v;
			//assimp supports upto 8 UV maps but we're going to assume only one exists for now
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 u;
				u.x = mesh->mTextureCoords[0][i].x;
				u.y = mesh->mTextureCoords[0][i].y;
				vertex.uv = u;
			}

			verts.push_back(vertex);
		}

		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			auto face = mesh->mFaces[i];
			for (auto j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[i]);
			}
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuse = loadTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuse.begin(), diffuse.end());

			std::vector<Texture> specular = loadTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specular.begin(), specular.end());

			std::vector<Texture> normal = loadTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normal.begin(), normal.end());

		}

		return Mesh(verts, indices, textures);
	}

	std::vector<Texture> Model::loadTextures(aiMaterial* mat, 
		aiTextureType type, std::string name)
	{
		std::vector<Texture> textures;
		for (auto i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (auto j = 0; j < loadedTextures.size(); j++)
			{
				if (std::strcmp(loadedTextures[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(loadedTextures[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				Texture tex;
				tex.id = textureFromFile(str.C_Str(), dir);
				tex.type = name;
				tex.path = str.C_Str();
				textures.push_back(tex);
				loadedTextures.push_back(tex);
			}
		}
		return textures;
	}

}