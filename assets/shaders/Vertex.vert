#version 450
layout(location = 0) in vec4 pos_in;
layout(location = 1) in vec3 normal_in;
layout(location = 0) out vec4 pos;
layout(location = 1) out vec3 normal;
layout(location = 2, binding = 0) uniform per_object
{
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
} obj;

void main() 
{
	pos = obj.projection * obj.view * obj.model * pos_in;
	normal = mat3(transpose(inverse(obj.model))) * normal_in;
			/*This transpose and inverse should be done on the CPU and then passed to the shader*/

	gl_Position = obj.projection * obj.view * obj.model * pos_in;
	gl_Position.y = -gl_Position.y; //Flip that shit! //Why tho?
	gl_Position.z = -gl_Position.z;
}
