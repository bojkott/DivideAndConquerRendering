#version 450
// inputs
layout (location = 0) out vec4 fragment_color;
layout (location = 0) in vec3 objectColor;	//Should maby be 2 and not 0.
layout (location = 1) in vec4 pos;
layout (location = 2) in vec3 normal;

layout(push_constant) uniform DIFFUSE_TINT_NAME
{
	layout(offset=16) vec4 diffuseTint;
};

void main() {
	vec3 lightPos = vec3(0.0, 10.0, -10.0);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - pos.xyz);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 result = (ambient + diffuse) * objectColor;



	fragment_color = vec4(result, 1.0);
}



