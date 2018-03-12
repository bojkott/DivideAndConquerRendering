#version 450

// inputs
layout(binding = 2) uniform Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 emission;
	vec4 specular;
	bool hasMask;
} material;


layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in float deviceId;
layout(location = 3) in vec4 outCamPos;
layout(location = 4) in vec3 outWorldPos;
layout (location = 0) out vec4 fragment_color;

//Textures
layout (binding = 3) uniform sampler2D diffuse;
layout (binding = 4) uniform sampler2D ambient;
layout (binding = 5) uniform sampler2D specular;
layout (binding = 6) uniform sampler2D specularHighlight;
layout (binding = 7) uniform sampler2D bump;
layout (binding = 8) uniform sampler2D displacement;
layout (binding = 9) uniform sampler2D alpha;
layout (binding = 10) uniform sampler2D reflecton;

void main() {
	vec3 lightPos = vec3(0.0, 10.0, -10.0);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 ambientAlbedo = (texture(ambient, inTexCoord) * material.ambient).rgb;
	vec3 diffuseAlbedo = (texture(diffuse, inTexCoord) * material.diffuse).rgb;
	float specularStrength = (texture(specular, inTexCoord)* material.specular).r;
	float alphaAlbedo = 1.0f;

	if(material.hasMask)
	{
		alphaAlbedo = texture(alpha, inTexCoord).r;
	}
		


	vec3 norm = normalize(inNorm);
	vec3 lightDir = vec3(0,1,0);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * diffuseAlbedo;

	// Specularpart
	vec3 viewDir = normalize(outCamPos.xyz - outWorldPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	vec3 reflectDir = reflect(-lightDir, norm); 

	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), 32);

	vec3 specular = specularStrength * spec * lightColor;  
	vec3 result = (diffuse + specular + ambientAlbedo);
	//diffuse += vec3(0.2f, 0, 0)*deviceId;

	//diffuse += vec3(0.0, 0.2f, 0)*(1-deviceId);

	fragment_color = vec4(result, alphaAlbedo);
}