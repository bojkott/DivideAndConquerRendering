#include "VertexBuffer.h"
#include "VulkanHelpers.h"

VertexBuffer::VertexBuffer(std::vector<uint32_t>& verts, DeviceContext * context)
{
	//vk::BufferCreateInfo bufferInfo = {};
	vk::DeviceSize bufferSize = sizeof(verts[0] * verts.size());

	//bufferInfo.size = bufferSize;
	//bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
	//bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	//vertexBuffer = context->getDevice().createBuffer(bufferInfo, nullptr);

	//// Find out what we need
	//vk::MemoryRequirements memRequirements;
	//context->getDevice().getBufferMemoryRequirements(vertexBuffer, &memRequirements);

	//// Find out what we got

	//vk::MemoryAllocateInfo allocInfo = {};
	//allocInfo.memoryTypeIndex = context->findMemoryType(memRequirements.memoryTypeBits,
	//	vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	//allocInfo.allocationSize = memRequirements.size;
	//

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	VulkanHelpers::createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		stagingBuffer, stagingBufferMemory, *context);
	


	//vertexBufferMemory = context->getDevice().allocateMemory(allocInfo);

	//context->getDevice().bindBufferMemory(stagingBuffer, stagingBufferMemory, 0);

	// Map to cpu 
	void* data;
	if (verts.size() != 0)
	{
		data = context->getDevice().mapMemory(stagingBufferMemory, 0, bufferSize);
		memcpy(data, verts.data(), (size_t)bufferSize);
		context->getDevice().unmapMemory(stagingBufferMemory);


		// Create the actual buffer
		VulkanHelpers::createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst
			| vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal,
			vertexBuffer, vertexBufferMemory, *context);


		context->executeSingleTimeQueue(
			[this, bufferSize, stagingBuffer](vk::CommandBuffer commandBuffer)
		{
			VulkanHelpers::cmdCopyBuffer(commandBuffer, stagingBuffer, vertexBuffer, bufferSize);
		});


		device->destroyBuffer(stagingBuffer);
		device->freeMemory(stagingBufferMemory);

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

