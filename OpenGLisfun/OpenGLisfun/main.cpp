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
#include "ContainerProg.h"
#include "Camera.h"
#include "Lamp.h"
#include "Model.h"
#include "Shader.h"
#include "BackCamera.h"
#include "ModelWithLight.h"
#include "SkyBox.h"
#include "Constants.h"

#include <glm\glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>

// global variables
static GLFWwindow *window;
// objects in the world
static ContainerProg *cp;
static Lamp *l;
static BackCamera *bc;
static Camera *c;
static SkyBox *sb;
static Model *m;
// other stuff
static int width, height;
static GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
static GLfloat lastFrame = 0.0f;  	// Time of last frame
static bool keys[1024];
static const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

// some function headers
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
bool createGLFWwindow();
void do_movement();
void render();
GLuint setup_uniformblockmatrices();
void setup_depthmap(GLuint &depthmapFB, GLuint &depthmap);

int main(int argc, char *argv[])
{
	createGLFWwindow();

	// create objects
	c = new Camera();
	l = new Lamp();
	bc = new BackCamera();
	sb = new SkyBox();
	cp = new ContainerProg(l);

	m = new ModelWithLight("..\\OpenGLisfun\\nanosuit\\nanosuit.obj", Shader("..\\OpenGLisfun\\loader.vert", "..\\OpenGLisfun\\loader.frag"), l);
	
	// setup
	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST);
	GLuint matricesUniformBlock = setup_uniformblockmatrices();
	GLuint shadowmapFB, shadowmap;
	setup_depthmap(shadowmapFB, shadowmap);
	cp->shadowMap = shadowmap;
	bc->texture = shadowmap;

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// check and call events
		glfwPollEvents();
		do_movement();

		// rendering
		glm::mat4 lightView = glm::lookAt(glm::vec3(0.0f, 2.0f, -1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		GLfloat near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		// 0. render into shadowmap, use lightProjection and lightView
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFB);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glm::mat4 view = c->GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, matricesUniformBlock);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(lightProjection));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightView));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::mat4), glm::value_ptr(glm::mat4(0.0)));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		render();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 1. render into frame buffer object for backcamera, use normal view and projection
		glBindFramebuffer(GL_FRAMEBUFFER, bc->FBO);
		glViewport(0, 0, width, height);
		Camera camera2 = *c;
		camera2.Front *= -1.0f;
		view = camera2.GetViewMatrix();
		glm::mat4 projection = glm::perspective(c->Zoom, (float)width / height, 0.1f, 100.0f);
		glBindBuffer(GL_UNIFORM_BUFFER, matricesUniformBlock);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::mat4), glm::value_ptr(lightSpaceMatrix));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// 2. render again into the default frame buffer
		view = c->GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, matricesUniformBlock);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		render();
		bc->render();	// render back camera

		// swap the front and back buffer
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthMask(GL_FALSE);
	sb->render();
	glDepthMask(GL_TRUE);

	cp->render();
	l->render();
	glEnable(GL_CULL_FACE);
	//m->shader = Shader("..\\OpenGLisfun\\loader.vert", "..\\OpenGLisfun\\loader.frag");
	m->Render();
	glDisable(GL_CULL_FACE);

	//m->shader = Shader("..\\OpenGLisfun\\loader.vert", "..\\OpenGLisfun\\geometrytest.frag", "..\\OpenGLisfun\\geometrytest.geom");
	//m->Render();
}

GLuint setup_uniformblockmatrices()
{
	GLuint matrices;

	glGenBuffers(1, &matrices);
	glBindBuffer(GL_UNIFORM_BUFFER, matrices);
	glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	glBindBufferRange(GL_UNIFORM_BUFFER, Constants::UniformMatricesBindingPoint, matrices, 0, 3 * sizeof(glm::mat4));

	return matrices;
}

void setup_depthmap(GLuint &depthmapFB, GLuint &depthmap)
{
	glGenFramebuffers(1, &depthmapFB);

	glGenTextures(1, &depthmap);
	glBindTexture(GL_TEXTURE_2D, depthmap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
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
