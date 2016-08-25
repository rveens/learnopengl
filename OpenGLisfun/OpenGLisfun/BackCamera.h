#pragma once
#include "Shader.h"

class BackCamera
{
public:
	GLuint FBO, texture;
	Shader shader = Shader("..\\OpenGLisfun\\backcamera.vert", "..\\OpenGLisfun\\backcamera.frag");

	BackCamera();
	virtual ~BackCamera();

	void render();
private:
	GLuint VAO, VBO;
	GLfloat data[5*6] {
		// vertex				// texcoords
		-1.0,	-1.0,	0.0,	0.0, 0.0,		// bottom left		// #1
		1.0,	-1.0,	0.0,	1.0, 0.0,		// bottom right
		-1.0,	1.0,	0.0,	0.0, 1.0,		// top left
		1.0,	-1.0,	0.0,	1.0, 0.0,		// bottom right		// #2
		-1.0,	1.0,	0.0,	0.0, 1.0,		// top left
		1.0,	1.0,	0.0,	1.0, 1.0,		// top right
	};
	void setup();
};

