#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

in VS_OUT {
	vec3 normal;
} gs_in[];

void generateline(int index)
{
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0f) * 0.01;
	EmitVertex();
	EndPrimitive();
}

void main()
{
	generateline(0);
	generateline(1);
	generateline(2);
}