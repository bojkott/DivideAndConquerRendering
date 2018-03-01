#include "Buffer.h"
#include "DeviceContext.h"
Buffer::Buffer(DeviceContext * deviceContext, size_t bufferSize, vk::BufferUsageFlags usage, vk::SharingMode sharingMode, vk::MemoryPropertyFlags properties)
{
	this->deviceContext = deviceContext;
	this->bufferSize = bufferSize;
	createBuffer(bufferSize, usage, sharingMode, properties);
}

Buffer::~Buffer()
{
	deviceContext->getDevice().freeMemory(bufferMemory);
	deviceContext->getDevice().destroyBuffer(buffer);
}

vk::Buffer & Buffer::getBuffer()
{
	return buffer;
}

void Buffer::transferBufferTo(Buffer & destination)
{
	void* dataA = this->deviceContext->getDevice().mapMemory(this->bufferMemory, 0, bufferSize);

	void* dataB = destination.deviceContext->getDevice().mapMemory(destination.bufferMemory, 0, bufferSize);

//	memcpy(dataB, dataA, bufferSize);

	this->deviceContext->getDevice().unmapMemory(this->bufferMemory);
	destination.deviceContext->getDevice().unmapMemory(destination.bufferMemory);
}

void Buffer::setData(void * data)
{
	void* bufferData = this->deviceContext->getDevice().mapMemory(this->bufferMemory, 0, bufferSize);
	memcpy(bufferData, data, bufferSize);
	this->deviceContext->getDevice().unmapMemory(this->bufferMemory);
}

void Buffer::createBuffer(size_t bufferSize, vk::BufferUsageFlags usage, vk::SharingMode sharingMode, vk::MemoryPropertyFlags properties)
{
	vk::BufferCreateInfo bufferInfo;
	bufferInfo.size = bufferSize;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = sharingMode;
	buffer = deviceContext->getDevice().createBuffer(bufferInfo, nullptr);

	vk::MemoryRequirements memRequirements;
	memRequirements = deviceContext->getDevice().getBufferMemoryRequirements(buffer);

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = deviceContext->findMemoryType(memRequirements.memoryTypeBits, properties);


	bufferMemory = deviceContext->getDevice().allocateMemory(allocInfo);
	deviceContext->getDevice().bindBufferMemory(buffer, bufferMemory, 0);
}
