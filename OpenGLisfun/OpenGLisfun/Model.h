#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include "Constants.h"

class Model
{
public:
	/* Functions */
	Model(GLchar *path, Shader shader) : shader(shader)
	{
		this->loadModel(path);
		// setup uniform buffer
		shader.Use();
		glUniformBlockBinding(shader.Program, glGetUniformBlockIndex(shader.Program, "Matrices"), Constants::UniformMatricesBindingPoint);
		glUseProgram(0);
	}
	virtual ~Model();
	void Render();
	virtual void setupUniforms() = 0;

	Shader shader;
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

