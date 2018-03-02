#version 450

layout(location = 0) out vec4 outColor;
layout(input_attachment_index = 0, binding=0, set=0) uniform subpassInput mainGpuDepth;
layout(input_attachment_index = 1, binding=1, set=0) uniform subpassInput slaveGpuColor;
layout(input_attachment_index = 2, binding=2, set=0) uniform subpassInput slaveGpuDepth;


void main() {
	float mainDepth = subpassLoad(mainGpuDepth).r;
	float slaveDepth = subpassLoad(slaveGpuDepth).r;

	if(mainDepth < slaveDepth)
			discard;
		else
			outColor = subpassLoad(slaveGpuColor);

    
}