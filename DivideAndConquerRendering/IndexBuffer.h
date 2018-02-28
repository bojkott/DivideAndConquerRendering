#pragma once
#include <vulkan/vulkan.hpp>
#include <SDL_vulkan.h>
#include "Buffer.h"
class IndexBuffer : public Buffer
{
private:
	uint32_t nrOfIndices;

public:
	IndexBuffer(std::vector<uint32_t>& indices, DeviceContext* context);
	void bind(vk::CommandBuffer& commandBuffer) const;
	uint32_t getNrOfIndices() const;
private:



};