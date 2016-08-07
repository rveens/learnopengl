#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal; 
layout (location = 2) in vec2 texcoord; 


out vec3 FragPos;
out vec2 Texcoord; 
out vec3 Result;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
  

uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
	vec3 FragPos = vec3(view * model * vec4(position, 1.0f));
	Texcoord = vec2(texcoord.x, 1.0f - texcoord.y);

	// ambient
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	float specularStrength = 1.0f;
	vec3 viewPos = vec3(0.0, 0.0, 0.0);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;
	
	Result = (ambient + diffuse + specular);
	//vec3 result = (ambient + diffuse + specular) * texture(bram, Texcoord).xyz;
}