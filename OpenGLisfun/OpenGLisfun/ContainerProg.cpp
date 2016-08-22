#include "ContainerProg.h"
#include "Constants.h"
#include "SOIL.h"
#include <glm/gtc/type_ptr.hpp>
#include <GLFW\glfw3.h>

ContainerProg::ContainerProg(Lamp *p) : p(p)
{
	setupShaders();
	setupVOA();
}

ContainerProg::~ContainerProg()
{
}

void ContainerProg::render()
{
	shader.Use();
		glm::mat4x4 model;
		model = glm::mat4();
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		/*glUniform3f(glGetUniformLocation(shader.Program, "material.ambient"), 0.0215f, 0.1745f, 0.0215f);
		glUniform3f(glGetUniformLocation(shader.Program, "material.diffuse"), 0.07568f, 0.61424f, 0.07568f);
		glUniform3f(glGetUniformLocation(shader.Program, "material.specular"), 0.633f, 0.727811f, 0.633f);*/
		glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 32.0f);

		glUniform3f(glGetUniformLocation(shader.Program, "dir.ambient"),	0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(shader.Program, "dir.diffuse"),	1.0f, 1.0f, 1.0f); // Let's darken the light a bit to fit the scene
		glUniform3f(glGetUniformLocation(shader.Program, "dir.specular"),	1.0f, 1.0f, 1.0f);

		glUniform3f(glGetUniformLocation(shader.Program, "point.ambient"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(shader.Program, "point.diffuse"), 0.8f, 0.8, 0.8);
		glUniform3f(glGetUniformLocation(shader.Program, "point.specular"), 0.7f, 0.7f, 0.7f);
	
		// light object
		glUniform3f(glGetUniformLocation(shader.Program, "LightPosition"), p->lightPos.x, p->lightPos.y, p->lightPos.z);
		glUniform1f(glGetUniformLocation(shader.Program, "point.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader.Program, "point.linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader.Program, "point.quadratic"), 0.032);

		// light directional
		//glUniform3f(glGetUniformLocation(m_program, "dir.direction"), 0.0f, 1.0f, 0.0f);
	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shader.Program, "material.diffuse"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(glGetUniformLocation(shader.Program, "material.specular"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glUniform1i(glGetUniformLocation(shader.Program, "material.emission"), 2);
	
		glBindVertexArray(m_VAO);
			for (GLuint i = 0; i < 10; i++)
			{
				model = glm::mat4();
				model = glm::translate(model, cubePositions[i]);
				GLfloat angle = 20.0f * i;
				model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
				glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		glBindVertexArray(0);
	glUseProgram(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ContainerProg::setupShaders()
{
	shader.Use();
		int width, height;
		unsigned char *image = SOIL_load_image("..\\OpenGLisfun\\container2.png", &width, &height, 0, SOIL_LOAD_RGB);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glUniform1i(glGetUniformLocation(shader.Program, "material.diffuse"), 0);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);

		image = SOIL_load_image("..\\OpenGLisfun\\container2_specular.png", &width, &height, 0, SOIL_LOAD_RGB);
		glGenTextures(1, &texture2);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glUniform1i(glGetUniformLocation(shader.Program, "material.specular"), 1);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);

		image = SOIL_load_image("..\\OpenGLisfun\\matrix.jpg", &width, &height, 0, SOIL_LOAD_RGB);
		glGenTextures(1, &texture3);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glUniform1i(glGetUniformLocation(shader.Program, "material.emission"), 2);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);


		glUniformBlockBinding(shader.Program, glGetUniformBlockIndex(shader.Program, "Matrices"), Constants::UniformMatricesBindingPoint);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ContainerProg::setTexture(GLuint textureHandle)
{
	static GLuint original = texture;
	if (textureHandle == 0)
		texture = original;
	else {
		texture = textureHandle;
	}
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
