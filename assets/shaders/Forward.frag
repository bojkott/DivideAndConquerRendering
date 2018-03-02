#version 450

// inputs
layout(binding = 2) uniform Material
{
	vec3 ambient;
	vec3 objColor;
} material;

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in vec3 inNorm;

layout (location = 0) out vec4 fragment_color;

void main() {
	vec3 lightPos = vec3(0.0, 10.0, -10.0);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 ambient = material.ambient * lightColor;

	vec3 norm = normalize(inNorm);
	vec3 lightDir = vec3(0,1,0);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 result = (material.ambient + diffuse) * material.objColor;
	

	fragment_color = vec4(result, 1.0);
	fragment_color = vec4(1,0,0,1);
}



