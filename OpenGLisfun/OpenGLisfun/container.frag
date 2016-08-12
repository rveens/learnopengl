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

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewPos = vec3(0.0, 0.0, 0.0);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = calculatePointLight(point, norm, viewDir);

	vec3 emission = texture(material.emission,Texcoord).xyz;
	
	color = vec4(result, 1.0);
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
