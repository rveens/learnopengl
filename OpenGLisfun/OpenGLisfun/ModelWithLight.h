#pragma once
#include "Model.h"
#include "Lamp.h"

class ModelWithLight :	public Model
{
public:
	ModelWithLight(GLchar *path, Shader shader, Lamp *l);
	virtual ~ModelWithLight();

	virtual void setupUniforms();
private:
	Lamp *l;
};

