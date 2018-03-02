#include "UniformBuffer.h"
#include "DeviceContext.h"

UniformBuffer::UniformBuffer(DeviceContext * deviceContext, size_t bufferSize)
	: Buffer(deviceContext, bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
{
	bufferInfo.buffer = buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = bufferSize;
}

void UniformBuffer::bind(int binding, vk::DescriptorSet & descriptorSet)
{
	vk::WriteDescriptorSet descriptorWrite;
	descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;

	deviceContext->getDevice().updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}