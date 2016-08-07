#pragma once
#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include "util.h"
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class OpenGLProg {
private:
	GLuint m_shaders[2];
	GLuint m_VBO;
	GLuint m_EBO;
	GLuint m_VAO;
	GLfloat m_vertices[32] = {
		// Positions          // Colors           // Texture Coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top Right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom Right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom Left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Top Left 
	};
	GLuint m_indices[9] = {  // Note that we start from 0!
		0, 1, 3,   // First Triangle
		1, 2, 3    // Second Triangle
	};
	GLuint texture;
	GLuint texture2;
	float aspectRatio;
	glm::vec3 m_cubePositions[10] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


public:
	GLuint m_program;

	OpenGLProg(float aspectRatio) : aspectRatio(aspectRatio)
	{
		setupShaders();
		setupTextures();
		setupVAO();
		setupUniformsStatic();
	}

	~OpenGLProg()
	{
	}

	void render()
	{
		glUseProgram(m_program);

		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(0.5f, 0.5f, 0.0f));
		trans = glm::rotate(trans, (GLfloat)(glfwGetTime() * 3.14), glm::vec3(0.0, 0.0, 1.0));
		trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

		glUniformMatrix4fv(glGetUniformLocation(m_program, "transform"), 1, GL_FALSE, glm::value_ptr(trans));


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glBindVertexArray(m_VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
private:
	void setupUniformsStatic()
	{
		glUseProgram(m_program);
			glUniform1f(glGetUniformLocation(m_program, "mixer"), 0.2f);

			glUniform1i(glGetUniformLocation(m_program, "doos"), 0);
			glUniform1i(glGetUniformLocation(m_program, "smiley"), 1);

			glm::mat4 model, view, projection;
			model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0, 0.0, 0.0));
			view = glm::translate(view, glm::vec3(0.0, 0.0, -3.0));
			projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
			glUniformMatrix4fv(glGetUniformLocation(m_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(m_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUseProgram(0);
	}
	void setupShaders()
	{
		GLuint shaders[2];
		shaders[0] = shader::load("..\\OpenGLisfun\\triangle.vert", GL_VERTEX_SHADER, true);
		shaders[1] = shader::load("..\\OpenGLisfun\\triangle.frag", GL_FRAGMENT_SHADER, true);
		m_program = program::link_from_shaders(shaders, 2, true, true);
	}
	void setupTextures()
	{
		int width, height;
		unsigned char *image = SOIL_load_image("..\\OpenGLisfun\\container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		image = SOIL_load_image("..\\OpenGLisfun\\awesomeface.png", &width, &height, 0, SOIL_LOAD_RGB);
		glGenTextures(1, &texture2);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void setupVAO()
	{
		// store everything openGL needs for drawing in a Vertex Array Object (VAO).
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);

		glBindVertexArray(m_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, nullptr);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid*)(sizeof(GLfloat) * 3));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
};