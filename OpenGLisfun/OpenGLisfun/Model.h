#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

class Model
{
public:
	/* Functions */
	Model(GLchar *path)
	{
		this->loadModel(path);
	}
	virtual ~Model();
	void Draw(Shader shader);
private:
	/* Model Data */
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	/* Functions */
	void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, 
		std::string typeName);
	GLuint TextureFromFile(const char * path, std::string directory);
};

