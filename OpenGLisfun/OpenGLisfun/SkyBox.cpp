#include "SkyBox.h"

#include <SOIL.h>
#include <glm/gtc/type_ptr.hpp>

SkyBox::SkyBox(glm::mat4x4 &view, glm::mat4x4 &projection) : WorldObject(view, projection)
{
	setup();
}


SkyBox::~SkyBox()
{
}

void SkyBox::render()
{
	shader.Use();
	glBindVertexArray(VBO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glm::mat4 newView = glm::mat4(glm::mat3(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(newView));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1i(glGetUniformLocation(shader.Program, "skybox"), 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glUseProgram(0);
}

void SkyBox::setup()
{
	// VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, nullptr);
		glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glBindBuffer(GL_VERTEX_ARRAY, 0);
	glDisableVertexAttribArray(0);

	// cubemap texture
	std::array<const GLchar*, 6> faces;
	faces[0] = "..\\OpenGLisfun\\skybox\\right.jpg";
	faces[1] = "..\\OpenGLisfun\\skybox\\left.jpg";
	faces[2] = "..\\OpenGLisfun\\skybox\\top.jpg";
	faces[3] = "..\\OpenGLisfun\\skybox\\bottom.jpg";
	faces[4] = "..\\OpenGLisfun\\skybox\\back.jpg";
	faces[5] = "..\\OpenGLisfun\\skybox\\front.jpg";
	cubemapTexture = loadCubemap(faces);
}

GLuint SkyBox::loadCubemap(std::array<const GLchar*, 6> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
			);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}