#pragma once
#include <vulkan\vulkan.hpp>
class DeviceContext;
class Buffer {
private:
	vk::Buffer buffer;
	vk::DeviceMemory bufferMemory;
	DeviceContext* deviceContext;
	size_t bufferSize;
public:
	Buffer(DeviceContext* deviceContext, size_t bufferSize, vk::BufferUsageFlags usage, vk::SharingMode sharingMode, vk::MemoryPropertyFlags properties);
	vk::Buffer& getBuffer();
	
	void transferBufferTo(Buffer& destination);

private:
	void createBuffer(size_t bufferSize, vk::BufferUsageFlags usage, vk::SharingMode sharingMode, vk::MemoryPropertyFlags properties);
};