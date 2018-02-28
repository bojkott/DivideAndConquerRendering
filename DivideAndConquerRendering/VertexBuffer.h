#pragma once
#include <vulkan/vulkan.hpp>
#include <SDL_vulkan.h>
#include "DeviceContext.h"
#include "Common.h"

class VertexBuffer
{
private:
	vk::DeviceMemory vertexBufferMemory;
	vk::Buffer vertexBuffer;
	uint32_t nrOfVerts;

	// Pointer to the device that the buffer belongs to
	vk::Device* device;
		
public:
	VertexBuffer(std::vector<Vertex>& verts, DeviceContext * context);
	VertexBuffer() {};  // instead of writing init functions
	void Destroy();
	vk::Buffer& getVertexBuffer();
	vk::DeviceMemory& getVertexBufferMemory();
	void BindBuffer(DeviceContext* context) const;
	void unBindBuffer(DeviceContext* context) const;
	uint32_t getNrOfVerts() const;

private:

};