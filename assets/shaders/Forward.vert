#version 450

layout(binding = 0) uniform Vertex
{
	vec3 pos;
	vec2 texCoord;
	vec3 norm;
} vertex;

layout(binding = 1) uniform camera
{
	uniform mat4 view;
	uniform mat4 projection;
} cam;

layout(binding = 2) uniform Material
{
	vec3 ambient;
	vec3 objColor;
} material;

layout(push_constant) uniform ModelMat
{
	mat4 model;
} modelMat;

layout(location = 0) out Vertex
{
	vec4 pos;
	vec2 texCoord;
	vec3 norm;
} outVertex;

void main() 
{
	vec4 pos = vec4(vertex.pos, 1.0);
	outVertex.pos = cam.projection * cam.view * modelMat.model * pos;
	outVertex.norm = mat3(transpose(inverse(modelMat.model))) * vertex.norm;
	outVertex.texCoord = vertex.texCoord;

	gl_Position = cam.projection * cam.view * modelMat.model * pos;
	gl_Position.y = -gl_Position.y; //Flip that shit! //Why tho?
	gl_Position.z = -gl_Position.z;
}
