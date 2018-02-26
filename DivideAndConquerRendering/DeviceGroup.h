#pragma once
#include <vector>
#include <memory>
#include <vulkan\vulkan.hpp>
#include "VkGroups.h"
#include "Shader.h"

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

	void createGraphicsPipeline(vk::PipelineCache pipelineCache, const vk::GraphicsPipelineCreateInfo & createInfo, vkGroups::PipelineGroup& pipelineGroup, vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);
	void createDescriptorPool(const vk::DescriptorPoolCreateInfo& poolInfo, vkGroups::DescriptorPoolGroup& descriptorPoolGroup, vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);

	void createDescriptorSetLayout(const vk::DescriptorSetLayoutCreateInfo& createInfo, vkGroups::DescriptorSetLayoutGroup& createDescriptorSetLayoutGroup, vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);

	void allocateDescriptorSet(const vkGroups::DescriptorPoolGroup& descriptorPool, const vkGroups::DescriptorSetLayoutGroup& descriptorSetLayout, vkGroups::DescriptorSetGroup& descriptorSetGroup);

	void createPipelineLayout(vk::PipelineLayoutCreateInfo& pipelineLayoutInfo, const vkGroups::DescriptorSetLayoutGroup& descriptorSetLayout, vkGroups::PipelineLayoutGroup& layoutGroup,
		vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);

	void createShaderGroup(const std::string& shaderFilename, Shader::ShaderType shaderType, vkGroups::ShaderGroup& shaderGroup);
	void createPipelineCache(vkGroups::PipelineCacheGroup& group);
	void createPipeline(vk::GraphicsPipelineCreateInfo& pipelineInfo,
		vkGroups::PipelineCacheGroup pipelineCacheGroup,
		vkGroups::ShaderGroup vertexShader,
		vkGroups::ShaderGroup fragmentShader,
		vkGroups::PipelineLayoutGroup pipelineLayoutGroup,
		vkGroups::PipelineGroup& group,
		vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);

};