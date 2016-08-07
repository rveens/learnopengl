#version 330 core

//in vec3 Normal;
//in vec3 FragPos;
in vec2 Texcoord;
in vec3 Result;

out vec4 color;

uniform vec3 objectColor;
uniform sampler2D bram;

void main()
{
    color = vec4(Result * objectColor, 1.0);
}