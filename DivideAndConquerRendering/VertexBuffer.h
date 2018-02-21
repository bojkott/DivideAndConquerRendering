#pragma once
#include <vulkan/vulkan.hpp>
#include <SDL_vulkan.h>
#include "DeviceContext.h"


class VertexBuffer
{
public:
	struct BufferData
	{
		vk::Buffer VertexBuffer;
		vk::DeviceMemory VertexMemory;
		BufferData(vk::Buffer VertexBuffer, vk::DeviceMemory VertexMemory) : 
			VertexBuffer(VertexBuffer), VertexMemory(VertexMemory) {};
	};


private:


public:
	static vk::Buffer CreateBuffer(std::vector<uint32_t> verts, DeviceContext& device);
	
private:
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

};