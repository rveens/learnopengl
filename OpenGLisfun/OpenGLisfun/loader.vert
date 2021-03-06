#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 LightPos;

uniform vec3 lightPos;

uniform mat4 model;
layout (std140) uniform Matrices 
{
	mat4 projection;
	mat4 view;
};

out VS_OUT {
	vec3 normal;
} vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    FragPos = vec3(view * model * vec4(position, 1.0f));
	Normal = normal;
	vs_out.normal = normal;
	TexCoords = texCoords;
	LightPos = vec3(view * vec4(lightPos, 1.0)); // Transform world-space light position to view-space light position
}