#pragma once
#include "Model.h"
#include "Lamp.h"

class ModelWithLight :	public Model
{
public:
	ModelWithLight(GLchar *path, Shader shader, glm::mat4x4 &view, glm::mat4x4 &projection, Lamp *l);
	virtual ~ModelWithLight();

	virtual void setupUniforms();
private:
	Lamp *l;
};

