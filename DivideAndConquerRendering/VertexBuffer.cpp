#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(std::vector<uint32_t>& verts, DeviceContext * context)
{
	vk::BufferCreateInfo bufferInfo = {};
	bufferInfo.size = sizeof(verts[0] * verts.size());
	bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	vertexBuffer = context->getDevice().createBuffer(bufferInfo, nullptr);

	// Find out what we need
	vk::MemoryRequirements memRequirements;
	context->getDevice().getBufferMemoryRequirements(vertexBuffer, &memRequirements);

	// Find out what we got

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.memoryTypeIndex = context->findMemoryType(memRequirements.memoryTypeBits,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	allocInfo.allocationSize = memRequirements.size;

	


	vertexBufferMemory = context->getDevice().allocateMemory(allocInfo);

	context->getDevice().bindBufferMemory(vertexBuffer, vertexBufferMemory, 0);

	// Map to cpu 
	void* data;
	if (verts.size() != 0)
	{
		data = context->getDevice().mapMemory(vertexBufferMemory, 0, bufferInfo.size);
		memcpy(data, verts.data(), (size_t)bufferInfo.size);
		context->getDevice().unmapMemory(vertexBufferMemory);
	}
	else
	{
		throw std::runtime_error("Inital data for vertexbuffer creation was nil");
	}

	// Everything worked, saving device 
	device = context->getAddressOfDevice();
}

void VertexBuffer::Destroy()
{
	device->destroyBuffer(vertexBuffer, nullptr);
	device->freeMemory(vertexBufferMemory, nullptr);
}

vk::Buffer& VertexBuffer::getVertexBuffer()
{
	return vertexBuffer;
}

vk::DeviceMemory& VertexBuffer::getVertexBufferMemory() 
{
	return vertexBufferMemory;
}

