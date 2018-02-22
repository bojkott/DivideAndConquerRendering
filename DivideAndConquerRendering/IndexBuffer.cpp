#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(std::vector<uint32_t> indices)
{
	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	
}
