#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 LightPos;   // Extra in variable, since we need the light position in view space we calculate this in the vertex shader

out vec4 color;
uniform sampler2D texture_specular1;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_diffuse1;


struct Material {
    //sampler2D	specular;
	//sampler2D	emission;
    float		shininess;
}; 
  
uniform Material material;

//uniform vec3 viewPos;

struct Light {
	//vec3 position;
	vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	   
	float constant;
    float linear;
    float quadratic;

	float cutOff;
};

uniform Light light;


void main()
{
	vec3 lightDir = normalize(LightPos - FragPos);
	vec3 norm = normalize(Normal);

	// ambient
	vec3 ambient = vec3(texture(texture_diffuse1,TexCoords)) * light.ambient;

	// diffuse
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = vec3(texture(texture_diffuse1,TexCoords)) * diff * light.diffuse;

	// specular
	vec3 viewDir = normalize(- FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = vec3(texture(texture_specular1,TexCoords)) * spec * light.specular;

	// attenuation
	float distance    = length(LightPos - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation;

	color = vec4(ambient + diffuse + specular, 1.0);
	//color = texture(texture_diffuse1,TexCoords);
}