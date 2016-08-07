#include "ContainerProg.h"
#include "util.h"
#include "SOIL.h"
#include <glm/gtc/type_ptr.hpp>
#include <GLFW\glfw3.h>

ContainerProg::ContainerProg(float aspectRatio) : aspectRatio(aspectRatio)
{
	setupShaders();
	setupVOA();
}

ContainerProg::~ContainerProg()
{
}

void ContainerProg::render(Camera *c, Lamp *p)
{
	glUseProgram(m_program);
	glm::mat4x4 model, view, projection;
	model = glm::mat4();
	view = c->GetViewMatrix();
	projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(m_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(m_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3f(glGetUniformLocation(m_program, "objectColor"), 1.0f, 0.5f, 0.31f);
	glUniform3f(glGetUniformLocation(m_program, "lightColor"), 1.0f, 1.0f, 1.0f); // Also set light's color (white)
	glm::vec4 lightPosView = view * model * p->lightPos;
	glUniform3f(glGetUniformLocation(m_program, "lightPos"), lightPosView.x, lightPosView.y, lightPosView.z);
	glUniform3f(glGetUniformLocation(m_program, "viewPos"), c->Position.x, c->Position.y, c->Position.z);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ContainerProg::setupShaders()
{
	GLuint shaders[2];
	shaders[0] = shader::load("..\\OpenGLisfun\\container.vert", GL_VERTEX_SHADER);
	shaders[1] = shader::load("..\\OpenGLisfun\\container.frag", GL_FRAGMENT_SHADER);
	m_program = program::link_from_shaders(shaders, 2, true);

	int width, height;
	unsigned char *image = SOIL_load_image("..\\OpenGLisfun\\capture.png", &width, &height, 0, SOIL_LOAD_RGB);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ContainerProg::setupVOA()
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices_cube), m_vertices_cube, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) (sizeof(GLfloat) * 3));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 6));
		glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
