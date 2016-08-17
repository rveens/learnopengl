#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal; 
layout (location = 2) in vec2 texcoord; 


out vec3 Normal;
out vec3 FragPos;
out vec2 Texcoord; 
out vec3 LightPos;

uniform mat4 model;
layout (std140) uniform Matrices 
{
	mat4 projection;
	mat4 view;
};

uniform vec3 LightPosition;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
	FragPos = vec3(view * model * vec4(position, 1.0f));
	Normal = normal;
	Texcoord = vec2(texcoord.x, texcoord.y);
	LightPos = vec3(view * vec4(LightPosition, 1.0));
}