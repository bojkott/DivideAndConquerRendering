#pragma once
#include <vulkan/vulkan.hpp>
#include <SDL_vulkan.h>
#include "DeviceContext.h"


class VertexBuffer
{
private:
	vk::DeviceMemory vertexBufferMemory;
	vk::Buffer vertexBuffer;

public:
	VertexBuffer(std::vector<uint32_t> verts, DeviceContext& device);
	~VertexBuffer();
	vk::Buffer& getVertexBuffer();
	vk::DeviceMemory& getVertexBufferMemory();

};