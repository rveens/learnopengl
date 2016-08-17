#include "ModelWithLight.h"

#include <glm/gtc/type_ptr.hpp>


ModelWithLight::ModelWithLight(GLchar *path, Shader shader, Lamp *l) : l(l), Model(path, shader)
{
}


ModelWithLight::~ModelWithLight()
{
}

void ModelWithLight::setupUniforms()
{
	// Draw the loaded model
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glUniform1f(glGetUniformLocation(shader.Program, "light.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(shader.Program, "light.linear"), 0.045);
	glUniform1f(glGetUniformLocation(shader.Program, "light.quadratic"), 0.0075);

	glUniform3f(glGetUniformLocation(shader.Program, "lightPos"), l->lightPos.x, l->lightPos.y, l->lightPos.z);
	//glUniform3f(glGetUniformLocation(shader.Program, "viewPos"), c->Position.x, c->Position.y, c->Position.z);

	glUniform3f(glGetUniformLocation(shader.Program, "light.ambient"), 0.1f, 0.1f, 0.1f);
	glUniform3f(glGetUniformLocation(shader.Program, "light.diffuse"), 0.8f, 0.8, 0.8);
	glUniform3f(glGetUniformLocation(shader.Program, "light.specular"), 0.7f, 0.7f, 0.7f);

	//glm::vec4 lightPos = l->lightPos;
	//glUniform3f(glGetUniformLocation(shader.Program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

}