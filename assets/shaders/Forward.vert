#version 450
layout(location = 0) in vec4 pos_in;
layout(location = 1) in vec3 normal_in;
layout(location = 0) out vec4 pos;
layout(location = 1) out vec3 normal;
layout(binding = 0) uniform UniformBufferObject
{
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
} ubo;

void main() 
{
	pos = ubo.projection * ubo.view * ubo.model * pos_in;
	normal = mat3(transpose(inverse(ubo.model))) * normal_in;
			/*This transpose and inverse should be done on the CPU and then passed to the shader*/

	gl_Position = ubo.projection * ubo.view * ubo.model * pos_in;
	gl_Position.y = -gl_Position.y; //Flip that shit! //Why tho?
	gl_Position.z = -gl_Position.z;
}
