#version 330 core

in vec2 TexCoord;
out vec4 color;

uniform sampler2D doos;
uniform sampler2D smiley;
uniform float mixer;

void main()
{
    color = mix(texture(doos, TexCoord), texture(smiley, vec2(-TexCoord.x, TexCoord.y)), 1.0); // * vec4(color_vert, 1.0);
} 