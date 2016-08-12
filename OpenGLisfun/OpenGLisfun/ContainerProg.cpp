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

	/*glUniform3f(glGetUniformLocation(m_program, "material.ambient"), 0.0215f, 0.1745f, 0.0215f);
	glUniform3f(glGetUniformLocation(m_program, "material.diffuse"), 0.07568f, 0.61424f, 0.07568f);
	glUniform3f(glGetUniformLocation(m_program, "material.specular"), 0.633f, 0.727811f, 0.633f);*/
	glUniform1f(glGetUniformLocation(m_program, "material.shininess"), 32.0f);

	glUniform3f(glGetUniformLocation(m_program, "dir.ambient"),	0.1f, 0.1f, 0.1f);
	glUniform3f(glGetUniformLocation(m_program, "dir.diffuse"),	1.0f, 1.0f, 1.0f); // Let's darken the light a bit to fit the scene
	glUniform3f(glGetUniformLocation(m_program, "dir.specular"),	1.0f, 1.0f, 1.0f);

	glUniform3f(glGetUniformLocation(m_program, "point.ambient"), 0.1f, 0.1f, 0.1f);
	glUniform3f(glGetUniformLocation(m_program, "point.diffuse"), 0.8f, 0.8, 0.8);
	glUniform3f(glGetUniformLocation(m_program, "point.specular"), 0.7f, 0.7f, 0.7f);
	
	// light object
	glUniform3f(glGetUniformLocation(m_program, "LightPosition"), p->lightPos.x, p->lightPos.y, p->lightPos.z);
	glUniform1f(glGetUniformLocation(m_program, "point.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(m_program, "point.linear"), 0.09);
	glUniform1f(glGetUniformLocation(m_program, "point.quadratic"), 0.032);

	// light directional
	//glUniform3f(glGetUniformLocation(m_program, "dir.direction"), 0.0f, 1.0f, 0.0f);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture3);
	
	glBindVertexArray(m_VAO);
		for (GLuint i = 0; i < 10; i++)
		{
			model = glm::mat4();
			model = glm::translate(model, cubePositions[i]);
			GLfloat angle = 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(glGetUniformLocation(m_program, "model"), 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	glBindVertexArray(0);
	glUseProgram(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ContainerProg::setupShaders()
{
	GLuint shaders[2];
	shaders[0] = shader::load("..\\OpenGLisfun\\container.vert", GL_VERTEX_SHADER);
	shaders[1] = shader::load("..\\OpenGLisfun\\container.frag", GL_FRAGMENT_SHADER);
	m_program = program::link_from_shaders(shaders, 2, true);

	glUseProgram(m_program);
		int width, height;
		unsigned char *image = SOIL_load_image("..\\OpenGLisfun\\container2.png", &width, &height, 0, SOIL_LOAD_RGB);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glUniform1i(glGetUniformLocation(m_program, "material.diffuse"), 0);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);

		image = SOIL_load_image("..\\OpenGLisfun\\container2_specular.png", &width, &height, 0, SOIL_LOAD_RGB);
		glGenTextures(1, &texture2);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glUniform1i(glGetUniformLocation(m_program, "material.specular"), 1);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);

		image = SOIL_load_image("..\\OpenGLisfun\\matrix.jpg", &width, &height, 0, SOIL_LOAD_RGB);
		glGenTextures(1, &texture3);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glUniform1i(glGetUniformLocation(m_program, "material.emission"), 2);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
	glUseProgram(0);
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
