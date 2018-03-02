#pragma once

#include "Buffer.h"

class DeviceContext;
class UniformBuffer : public Buffer
{
private:
	vk::DescriptorBufferInfo bufferInfo;
public:

	UniformBuffer(DeviceContext* deviceContext, size_t bufferSize);

	void bind(int binding, vk::DescriptorSet& descriptorSet);
};