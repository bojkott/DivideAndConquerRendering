#version 450

// inputs
layout(binding = 2) uniform Material
{
	vec3 ambient;
	vec3 objColor;
} material;


layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in float deviceId;

layout (location = 0) out vec4 fragment_color;

//Textures
layout (binding = 1) uniform sampler2D diffuse;
layout (binding = 2) uniform sampler2D ambient;
layout (binding = 3) uniform sampler2D specular;
layout (binding = 4) uniform sampler2D specularHighlight;
layout (binding = 5) uniform sampler2D bump;
layout (binding = 6) uniform sampler2D displacement;
layout (binding = 7) uniform sampler2D alpha;
layout (binding = 8) uniform sampler2D reflecton;

void main() {
	vec3 lightPos = vec3(0.0, 10.0, -10.0);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 ambient = vec3(0.5, 0.5f, 0.5f);

	vec3 norm = normalize(inNorm);
	vec3 lightDir = vec3(0,1,0);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * ambient + vec3(0.2f,0.2f,0.2f);

	vec3 result = (material.ambient + diffuse) * material.objColor;

	diffuse += vec3(0.2f, 0, 0)*deviceId;

	diffuse += vec3(0.0, 0.2f, 0)*(1-deviceId);

	fragment_color = vec4(diffuse, 1.0);
}