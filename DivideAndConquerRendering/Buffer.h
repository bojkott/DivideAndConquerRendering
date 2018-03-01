#pragma once
#include <vulkan\vulkan.hpp>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class DeviceContext;
class Buffer {
protected:
	vk::Buffer buffer;
	vk::DeviceMemory bufferMemory;
	DeviceContext* deviceContext;
	size_t bufferSize;
public:
	Buffer(DeviceContext* deviceContext, size_t bufferSize, vk::BufferUsageFlags usage, vk::SharingMode sharingMode, vk::MemoryPropertyFlags properties);
	Buffer() {};
	~Buffer();
	vk::Buffer& getBuffer();
	
	void transferBufferTo(Buffer& destination);
	void setData(void* data);
private:
	void createBuffer(size_t bufferSize, vk::BufferUsageFlags usage, vk::SharingMode sharingMode, vk::MemoryPropertyFlags properties);
};