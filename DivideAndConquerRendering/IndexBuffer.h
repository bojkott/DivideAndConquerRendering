#pragma once
#include <vulkan/vulkan.hpp>
#include <SDL_vulkan.h>
#include "Common.h"

class IndexBuffer
{
private:
	vk::Buffer indexBuffer;
	vk::DeviceMemory indexBufferMemory;

	uint32_t nrOfIndices;

	// Pointer to the device that the buffer belongs to
	vk::Device* device;

public:
	IndexBuffer(std::vector<uint32_t> indices, DeviceContext* context);
	void BindBuffer(DeviceContext * context) const;
	uint32_t getNrOfIndices() const;


private:



};