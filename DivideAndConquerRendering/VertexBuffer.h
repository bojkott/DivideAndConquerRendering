#pragma once
#include <vulkan/vulkan.hpp>
#include <SDL_vulkan.h>
#include "DeviceContext.h"


class VertexBuffer
{
private:
	vk::DeviceMemory vertexBufferMemory;
	vk::Buffer vertexBuffer;

	// Pointer to the device that the buffer belongs to
	vk::Device* device;
	
public:
	VertexBuffer(std::vector<uint32_t>& verts, DeviceContext * device);
	VertexBuffer() {}; // Just to make std::map stfu
	void Destroy();
	vk::Buffer& getVertexBuffer();
	vk::DeviceMemory& getVertexBufferMemory();

};