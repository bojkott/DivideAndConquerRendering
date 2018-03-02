#version 450

// inputs
layout(binding = 2) uniform Material
{
	vec3 ambient;
	vec3 objColor;
} material;

layout(location = 0) in Vertex
{
	vec4 pos;
	vec2 texCoord;
	vec3 norm;
} vertex;

layout (location = 0) out vec4 fragment_color;

void main() {
	vec3 lightPos = vec3(0.0, 10.0, -10.0);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 ambient = material.ambient * lightColor;

	vec3 norm = normalize(vertex.norm);
	vec3 lightDir = normalize(lightPos - vertex.pos.xyz);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 result = (material.ambient + diffuse) * material.objColor;



	fragment_color = vec4(result, 1.0);
}



