#include "VertexBuffer.h"

vk::Buffer VertexBuffer::CreateBuffer(std::vector<uint32_t> verts, DeviceContext& device)
{
	// TODO, return useful data
	vk::BufferCreateInfo bufferInfo = {};
	bufferInfo.size = sizeof(verts[0] * verts.size());
	bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	vk::Buffer vertexBuffer;
	vertexBuffer = device.getDevice().createBuffer(bufferInfo, nullptr);

	// Find out what we need
	vk::MemoryRequirements memRequirements;
	device.getDevice().getBufferMemoryRequirements(vertexBuffer, &memRequirements);

	// Find out what we got

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.memoryTypeIndex = device.findMemoryType(memRequirements.memoryTypeBits, 
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	allocInfo.allocationSize = memRequirements.size;

	vk::DeviceMemory vertexBufferMemory;


	vertexBufferMemory = device.getDevice().allocateMemory(allocInfo);

	device.getDevice().bindBufferMemory(vertexBuffer, vertexBufferMemory, 0);


	return vertexBuffer;
}

uint32_t VertexBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	return uint32_t();
}
