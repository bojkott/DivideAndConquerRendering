#include "IndexBuffer.h"
#include "DeviceContext.h"
#include "VulkanHelpers.h"

IndexBuffer::IndexBuffer(std::vector<uint32_t>& indices, DeviceContext* context)
	: Buffer(context, sizeof(indices[0])*indices.size(), vk::BufferUsageFlagBits::eIndexBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal)
{
	
	nrOfIndices = indices.size();
	if (indices.size() != 0)
	{

		Buffer stagingBuffer = Buffer(
			context,
			bufferSize,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::SharingMode::eExclusive,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		stagingBuffer.setData(indices.data());

		// Move data from staging to actual buffer
		context->executeSingleTimeQueue(
			[this, &stagingBuffer](vk::CommandBuffer commandBuffer)
		{
			VulkanHelpers::cmdCopyBuffer(commandBuffer, stagingBuffer.getBuffer(), getBuffer(), bufferSize);
		});

	}
	else
	{
		throw std::runtime_error("initial data for indexbuffer was nil");
	}
}

void IndexBuffer::bind(vk::CommandBuffer& commandBuffer) const
{
		vk::DeviceSize offsets[] = { 0 };
		commandBuffer.bindIndexBuffer(buffer, 0, vk::IndexType::eUint32);
}

inline uint32_t IndexBuffer::getNrOfIndices() const
{
	return nrOfIndices;
}
