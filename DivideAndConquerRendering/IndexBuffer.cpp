#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(std::vector<uint32_t> indices, DeviceContext* context)
{
	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	
	VulkanHelpers::createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		stagingBuffer, stagingBufferMemory, *context);

	if (indices.size() != 0)
	{
		nrOfIndices = indices.size();
		void* data;
		data = context->getDevice().mapMemory(stagingBufferMemory, 0, bufferSize);
		memcpy(data, indices.data(), (size_t)bufferSize);
		context->getDevice().unmapMemory(stagingBufferMemory);


		// Create the actual buffer
		VulkanHelpers::createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst
			| vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal,
			indexBuffer, indexBufferMemory, *context);

		context->executeSingleTimeQueue(
			[this, bufferSize, stagingBuffer](vk::CommandBuffer commandBuffer)
		{
			VulkanHelpers::cmdCopyBuffer(commandBuffer, stagingBuffer, indexBuffer, bufferSize);
		});


		// Everything worked, saving device 
		device = context->getAddressOfDevice();
		device->destroyBuffer(stagingBuffer);
		device->freeMemory(stagingBufferMemory);

	}
	else
	{
		// Free all resources preallocated on card if we throw
		context->getDevice().destroyBuffer(indexBuffer, nullptr);
		context->getDevice().freeMemory(stagingBufferMemory, nullptr);
		throw std::runtime_error("initial data for indexbuffer was nil");
	}
}



void IndexBuffer::BindBuffer(DeviceContext* context) const
{
	context->executeSingleTimeQueue(
		[this](vk::CommandBuffer commandBuffer)
	{
		vk::DeviceSize offsets[] = { 0 };
		commandBuffer.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint16);
	});
}

inline uint32_t IndexBuffer::getNrOfIndices() const
{
	return nrOfIndices;
}
