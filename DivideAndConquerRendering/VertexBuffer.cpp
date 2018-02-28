#include "VertexBuffer.h"
#include "VulkanHelpers.h"

VertexBuffer::VertexBuffer(std::vector<Vertex>& verts, DeviceContext * context)
{
	vk::DeviceSize bufferSize = sizeof(verts[0].pos.r * verts.size());
	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	VulkanHelpers::createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		stagingBuffer, stagingBufferMemory, *context);
	


	// Map to cpu 
	void* data;
	if (verts.size() != 0)
	{
		nrOfVerts = verts.size();
		data = context->getDevice().mapMemory(stagingBufferMemory, 0, bufferSize);
		memcpy(data, verts.data(), (size_t)bufferSize);
		context->getDevice().unmapMemory(stagingBufferMemory);


		// Create the actual buffer
		VulkanHelpers::createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst
			| vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal,
			vertexBuffer, vertexBufferMemory, *context);


		// Move data from staging to actual buffer
		context->executeSingleTimeQueue(
			[this, bufferSize, stagingBuffer](vk::CommandBuffer commandBuffer)
		{
			VulkanHelpers::cmdCopyBuffer(commandBuffer, stagingBuffer, vertexBuffer, bufferSize);
		});


		// Everything worked, saving device 
		device = context->getAddressOfDevice();
		device->destroyBuffer(stagingBuffer);
		device->freeMemory(stagingBufferMemory);

	}
	else
	{
		// Free all resources preallocated on card if we throw
		context->getDevice().destroyBuffer(stagingBuffer, nullptr);
		context->getDevice().freeMemory(stagingBufferMemory, nullptr);
		throw std::runtime_error("Inital data for vertexbuffer creation was nil");
	}

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

void VertexBuffer::BindBuffer(DeviceContext* context) const
{
	context->executeSingleTimeQueue(
		[this](vk::CommandBuffer commandBuffer)
	{
		vk::DeviceSize offsets[] = { 0 };
		commandBuffer.bindVertexBuffers(0, 1, &vertexBuffer, offsets);
	});
}

void VertexBuffer::unBindBuffer(DeviceContext* context) const
{
	// TODO: HOW DO I DO IT?
}

inline uint32_t VertexBuffer::getNrOfVerts() const
{
	return nrOfVerts;
}

