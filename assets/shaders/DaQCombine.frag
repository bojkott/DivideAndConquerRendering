#version 450

layout(location = 0) out vec4 outColor;
layout(input_attachment_index = 0, binding=0, set=0) uniform subpassInput inColor;

void main() {
    outColor = subpassLoad(inColor);
}