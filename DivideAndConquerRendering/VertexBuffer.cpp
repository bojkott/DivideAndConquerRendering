#include "VertexBuffer.h"
#include "VulkanHelpers.h"
#include "DeviceContext.h"

VertexBuffer::VertexBuffer(std::vector<ModelHelper::Vertex>& verts, DeviceContext * context)
	: Buffer(context, sizeof(verts[0].pos) * verts.size(), vk::BufferUsageFlagBits::eVertexBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal)
{
	nrOfVerts = verts.size();
	// Map to cpu 
	if (verts.size() != 0)
	{
		Buffer stagingBuffer = Buffer(
			context,
			bufferSize,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::SharingMode::eExclusive,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		stagingBuffer.setData(verts.data());

		// Move data from staging to actual buffer
		context->executeSingleTimeQueue(
			[this, &stagingBuffer](vk::CommandBuffer commandBuffer)
		{
			VulkanHelpers::cmdCopyBuffer(commandBuffer, stagingBuffer.getBuffer(), getBuffer(), bufferSize);
		});

	}
	else
	{
		throw std::runtime_error("Inital data for vertexbuffer creation was nil");
	}

}
void VertexBuffer::bind(vk::CommandBuffer& commandBuffer) const
{

	vk::DeviceSize offsets[] = { 0 };
	commandBuffer.bindVertexBuffers(0, 1, &buffer, offsets);
}

inline uint32_t VertexBuffer::getNrOfVerts() const
{
	return nrOfVerts;
}

