#version 330 core

in vec2 Texcoord;

out vec4 color;

uniform sampler2D tex;

// prototypes
vec3 applyKernel(float kernel[9], sampler2D tx, vec2 texcoords);

void main()
{
	// nieuw 
	float trippy[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
	float blur[9] = float[](
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16  
	);
	float edgy[9] = float[](
		1.0,	1.0,		1.0,
		1.0,	-8.0,	1.0,
		1.0,	1.0,	1.0
	);
	float sobel[9] = float[](
		-1.0,	-2.0,	-1.0,
		0.0,	0.0,	0.0,
		1.0,	2.0,	1.0
	);
	color = vec4(applyKernel(trippy, tex, Texcoord), 1.0);
	//color = vec4(vec3(texture(tex, Texcoord).r), 1.0);
}

vec3 applyKernel(float kernel[9], sampler2D tx, vec2 texcoords)
{
	const float offset = 1.0 / 300;  

	vec2 offsets[9] = vec2[](
        vec2(-offset, offset),  // top-left
        vec2(0.0f,    offset),  // top-center
        vec2(offset,  offset),  // top-right
        vec2(-offset, 0.0f),    // center-left
        vec2(0.0f,    0.0f),    // center-center
        vec2(offset,  0.0f),    // center-right
        vec2(-offset, -offset), // bottom-left
        vec2(0.0f,    -offset), // bottom-center
        vec2(offset,  -offset)  // bottom-right    
    );

	vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(tx, texcoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

	return col;
}