#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 Texcoord;
in vec3 LightPos;

out vec4 color;



struct Material {
    sampler2D	diffuse;
    sampler2D	specular;
	sampler2D	emission;
    float		shininess;
}; 
  
uniform Material material;

struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	   
	float constant;
    float linear;
    float quadratic;
};

uniform PointLight point;


// prototypes
vec3 calculatePointLight(PointLight light, vec3 norm, vec3 viewDir);
vec3 applyKernel(float kernel[9], sampler2D tex, vec2 texcoords);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(- FragPos);

	vec3 result = calculatePointLight(point, norm, viewDir);

	vec3 emission = texture(material.emission,Texcoord).xyz;
	
	color = vec4(result, 1.0);

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
	color = vec4(applyKernel(edgy, material.diffuse, Texcoord), 1.0);
}

vec3 applyKernel(float kernel[9], sampler2D tex, vec2 texcoords)
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
        sampleTex[i] = vec3(texture(tex, texcoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

	return col;
}


vec3 calculatePointLight(PointLight light, vec3 norm, vec3 viewDir)
{
	vec3 lightDir = normalize(FragPos - LightPos);

	// ambient
	vec3 ambient = vec3(texture(material.diffuse,Texcoord)) * light.ambient;

	// diffuse
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = vec3(texture(material.diffuse,Texcoord)) * diff * light.diffuse;

	// specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = vec3(texture(material.specular,Texcoord)) * spec * light.specular;

	// attenuation
	float distance    = length(LightPos - FragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation;

	return ambient + diffuse + specular;
}
