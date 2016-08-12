#include "Lamp.h"

#include "util.h"
#include <glm/gtc/type_ptr.hpp>
#include <GLFW\glfw3.h>

Lamp::Lamp(float aspectRatio) : aspectRatio(aspectRatio)
{
	setupShaders();
	setupVOA();
}

Lamp::~Lamp()
{
}

void Lamp::render(Camera *c)
{
	glUseProgram(m_program);
	glm::mat4x4 model, view, projection;
	model = glm::mat4();
	model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));
	model = glm::translate(model, offset);
	model = glm::scale(model, glm::vec3(0.2f));
	lightPos =  model * lightPos;
	view = c->GetViewMatrix();
	projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(m_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(m_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Lamp::setupShaders()
{
	GLuint shaders[2];
	shaders[0] = shader::load("..\\OpenGLisfun\\container.vert", GL_VERTEX_SHADER);
	shaders[1] = shader::load("..\\OpenGLisfun\\lamp.frag", GL_FRAGMENT_SHADER);
	m_program = program::link_from_shaders(shaders, 2, true);
}

void Lamp::setupVOA()
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices_cube), m_vertices_cube, GL_STATIC_DRAW);
	glGenBuffers(1, &m_EBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
