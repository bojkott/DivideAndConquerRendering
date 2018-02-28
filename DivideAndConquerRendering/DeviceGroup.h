#pragma once
#include <vector>
#include <memory>
#include <vulkan\vulkan.hpp>
#include "VkGroups.h"
#include "Shader.h"
#include "Model.h"


class DeviceContext;
class DeviceGroup {
private:
	std::vector<DeviceContext*> devices;

public:
	DeviceGroup();
	~DeviceGroup();
	void addDevice(vk::Instance& instance, vk::PhysicalDevice physicalDevice);
	void addDevice(vk::Instance& instance, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR& surface);
	
	void initDevices();

	unsigned int getGroupSize();
	DeviceContext* getMainDevice();
	std::vector<DeviceContext*> getDevices();

	void createShaderGroup(const std::string& shaderFilename, Shader::ShaderType shaderType, vkGroups::ShaderGroup& shaderGroup);


	vkGroups::BufferGroup createBuffer(vk::BufferCreateInfo bufferInfo, vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);
	vkGroups::BufferMemoryGroup allocateMemory(vkGroups::BufferGroup& bufferGroup, vk::MemoryPropertyFlags properies, vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);

	void bindBufferMemory(vkGroups::BufferGroup& bufferGroup, vkGroups::BufferMemoryGroup& bufferMemoryGroup, vk::DeviceSize memoryOffest);

};