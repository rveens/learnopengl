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
#include "util.h"
//#include "OpenGLProg.h"
#include "ContainerProg.h"
#include "Camera.h"
#include "Lamp.h"
#include "Model.h"
#include "Shader.h"
#include "ModelWithLight.h"
#include <glm\glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// global variables
static GLFWwindow *window;
static glm::mat4 projection, view;
static ContainerProg *cp;
static Lamp *l;
static Camera *c;
static int width, height;
static GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
static GLfloat lastFrame = 0.0f;  	// Time of last frame

// some function headers
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
bool createGLFWwindow();
void do_movement();

static bool keys[1024];

int main(int argc, char *argv[])
{
	createGLFWwindow();
	c = new Camera();
	//t = new OpenGLProg(width/height, c);
	l = new Lamp(view, projection);

	cp = new ContainerProg(view, projection);
	Shader shader("..\\OpenGLisfun\\loader.vert", "..\\OpenGLisfun\\loader.frag");
	Model *m = new ModelWithLight("..\\OpenGLisfun\\nanosuit\\nanosuit.obj", shader, view, projection, l);
	
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// check and call events
		glfwPollEvents();
		do_movement();

		// rendering
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection = glm::perspective(c->Zoom, (float)800.0 / (float)400.0, 0.1f, 100.0f);
		view = c->GetViewMatrix();

		l->render();
		//cp->render(c, l);
		m->Render();

		// swap the front and back buffer
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	static bool toggle = true;
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
		glUseProgram(cp->m_program);
		glUniform1f(glGetUniformLocation(cp->m_program, "mixer"), value);
		glUseProgram(0);
	}
	if (key == GLFW_KEY_RIGHT && GLFW_PRESS) {
		value += 0.1f;
		glUseProgram(cp->m_program);
		glUniform1f(glGetUniformLocation(cp->m_program, "mixer"), value);
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