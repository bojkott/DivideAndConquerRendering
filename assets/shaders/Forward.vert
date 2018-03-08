#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNorm;

layout(binding = 1) uniform camera
{
	mat4 view;
	mat4 projection;
	vec4 camPos;
	int deviceId;
} cam;

layout(binding = 2) uniform Material
{
	vec3 ambient;
	vec3 objColor;
} material;

layout(push_constant) uniform ModelMat
{
	layout(offset=0) mat4 model;
} modelMat;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out vec3 outNorm;
layout(location = 2) out float deviceId;
layout(location = 3) out vec4 outCamPos;

layout(location = 4) out vec4 outWorldPos;

void main() 
{
	vec4 pos = vec4(inPos, 1.0);
	outNorm = mat3(transpose(inverse(modelMat.model))) * inNorm;
	outTexCoord = inTexCoord;
	outCamPos = cam.camPos;
	outWorldPos = modelMat.model * pos;
	gl_Position = cam.projection * cam.view * modelMat.model * pos;
	//gl_Position.y = -gl_Position.y; //Flip that shit! //Why tho?

	deviceId = cam.deviceId;
}
