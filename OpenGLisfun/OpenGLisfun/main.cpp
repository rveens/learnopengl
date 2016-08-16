// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// SOIL
#include <SOIL.h>
// assimp
#define ASSIMP_BUILD_STATIC_LIB
// std
#include <iostream>
// this project
//#include "OpenGLProg.h"
#include "ContainerProg.h"
#include "Camera.h"
#include "Lamp.h"
#include "Model.h"
#include "Shader.h"
#include "BackCamera.h"
#include "ModelWithLight.h"
#include <glm\glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>

// global variables
static GLFWwindow *window;
static glm::mat4 projection, view;
static ContainerProg *cp;
static Lamp *l;
static BackCamera *bc;
static Camera *c;
static int width, height;
static GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
static GLfloat lastFrame = 0.0f;  	// Time of last frame

// some function headers
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
bool createGLFWwindow();
void do_movement();
void setup_framebuffer();
GLuint loadCubemap(std::array<const GLchar*, 6> faces);

static bool keys[1024];
static GLuint framebuffer, texturecolorbuffer;

int main(int argc, char *argv[])
{
	createGLFWwindow();
	c = new Camera();
	l = new Lamp(view, projection);
	bc = new BackCamera(view, projection);

	cp = new ContainerProg(view, projection, l);
	Shader shader("..\\OpenGLisfun\\loader.vert", "..\\OpenGLisfun\\loader.frag");
	Model *m = new ModelWithLight("..\\OpenGLisfun\\nanosuit\\nanosuit.obj", shader, view, projection, l);
	
	glEnable(GL_DEPTH_TEST);
	setup_framebuffer();
	std::array<const GLchar*, 6> faces;
	faces[0] = "..\\OpenGLisfun\\skybox\\right.jpg";
	faces[1] = "..\\OpenGLisfun\\skybox\\left.jpg";
	faces[2] = "..\\OpenGLisfun\\skybox\\top.jpg";
	faces[3] = "..\\OpenGLisfun\\skybox\\bottom.jpg";
	faces[4] = "..\\OpenGLisfun\\skybox\\back.jpg";
	faces[5] = "..\\OpenGLisfun\\skybox\\front.jpg";
	GLuint cubemapTexture = loadCubemap(faces);


	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// check and call events
		glfwPollEvents();
		do_movement();

		// rendering
		projection = glm::perspective(c->Zoom, (float)width / height, 0.1f, 100.0f);
		// 1. render into frame buffer object
		Camera camera2 = *c;
		camera2.Front *= -1.0f;
		view = camera2.GetViewMatrix();
		glBindFramebuffer(GL_FRAMEBUFFER, bc->FBO);
		glClearColor(0.0f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		l->render();
		glEnable(GL_CULL_FACE);
		m->Render();
		glDisable(GL_CULL_FACE);
		cp->render();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// 2. render again into the default frame buffer
		view = c->GetViewMatrix();
		glClearColor(0.0f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		l->render();
		glEnable(GL_CULL_FACE);
		m->Render();
		glDisable(GL_CULL_FACE);
		cp->render();

		bc->render();
		// swap the front and back buffer
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void setup_framebuffer()
{
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &texturecolorbuffer);
	glBindTexture(GL_TEXTURE_2D, texturecolorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texturecolorbuffer, 0);

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	static bool toggle = true;
	static bool toggle2 = true;
	static float value = 0.2f;
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		if (toggle)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		toggle = !toggle;
	}

	if (key == GLFW_KEY_LEFT && GLFW_PRESS) {
		value -= 0.1f;
		glUseProgram(cp->shader.Program);
		glUniform1f(glGetUniformLocation(cp->shader.Program, "mixer"), value);
		glUseProgram(0);
	}
	if (key == GLFW_KEY_RIGHT && GLFW_PRESS) {
		value += 0.1f;
		glUseProgram(cp->shader.Program);
		glUniform1f(glGetUniformLocation(cp->shader.Program, "mixer"), value);
		glUseProgram(0);
	}
	if (key == GLFW_KEY_P && GLFW_PRESS) {
		if (toggle2) {
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			cp->setTexture(texturecolorbuffer);
		}
		toggle2 = !toggle2;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	static GLfloat lastX = 400, lastY = 300;
	static bool firstMouse = true;
	static GLfloat yaw = -90.0f, pitch = 0.0, roll = 0.0;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.3;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	c->ProcessMouseMovement(xoffset, yoffset, true);
}

void do_movement()
{
	GLfloat cameraSpeed = 5.0f * deltaTime;

	if (keys[GLFW_KEY_W]) {
		c->ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A]) {
		c->ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_S]) {
		c->ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_D]) {
		c->ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	}
}

bool createGLFWwindow()
{
	// GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(800, 600, "OpenGLisfun", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glew
	//glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	// set opengl viewport
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	return true;
}

GLuint loadCubemap(std::array<const GLchar*, 6> faces)
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