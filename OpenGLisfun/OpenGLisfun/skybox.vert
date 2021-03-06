#version 330 core

layout (location = 0) in vec3 position;
out vec3 TexCoords;

layout (std140) uniform Matrices 
{
	mat4 projection;
	mat4 view;
};

void main()
{		
	mat4 newView = mat4(mat3(view));
	gl_Position = projection * newView * vec4(position, 1.0);
	TexCoords = position;
}