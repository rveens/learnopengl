#include "Lamp.h"
#include "Constants.h"

#include <glm/gtc/type_ptr.hpp>
#include <GLFW\glfw3.h>

Lamp::Lamp()
{
	setupVOA();
}

Lamp::~Lamp()
{
}

void Lamp::render()
{
	shader.Use();
		glm::mat4x4 model;
		model = glm::mat4();
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));
		model = glm::translate(model, offset);
		model = glm::scale(model, glm::vec3(0.2f));
		lightPos =  model * lightPos;
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(m_VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	glUseProgram(0);
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

	// setup uniform buffer
	shader.Use();
	glUniformBlockBinding(shader.Program, glGetUniformBlockIndex(shader.Program, "Matrices"), Constants::UniformMatricesBindingPoint);
	glUseProgram(0);
}
