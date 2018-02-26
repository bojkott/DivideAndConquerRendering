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

	vkGroups::PipelineGroup createGraphicsPipeline(vk::PipelineCache pipelineCache, const vk::GraphicsPipelineCreateInfo & createInfo, vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);
	vkGroups::DescriptorPoolGroup createDescriptorPool(const vk::DescriptorPoolCreateInfo& poolInfo, vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);
	vkGroups::DescriptorSetLayoutGroup createDescriptorSetLayout(const vk::DescriptorSetLayoutCreateInfo& createInfo, vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);
	vkGroups::DescriptorSetGroup allocateDescriptorSet(const vkGroups::DescriptorPoolGroup& descriptorPool, const vkGroups::DescriptorSetLayoutGroup& descriptorSetLayout);
	vkGroups::PipelineLayoutGroup createPipelineLayout(vk::PipelineLayoutCreateInfo& pipelineLayoutInfo, 
		const vkGroups::DescriptorSetLayoutGroup& descriptorSetLayout, vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);

	vkGroups::PipelineShaderStageGroup createPipelineShaderStage(const std::string& shaderFilename, Shader::Type shaderType);
	vkGroups::PipelineCacheGroup createPipelineCache();
	vkGroups::PipelineGroup createPipeline(vk::GraphicsPipelineCreateInfo& pipelineInfo,
		vkGroups::PipelineCacheGroup pipelineCacheGroup,
		vkGroups::PipelineShaderStageGroup vertexShader,
		vkGroups::PipelineShaderStageGroup fragmentShader,
		vkGroups::PipelineLayoutGroup pipelineLayoutGroup,
		vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);
	
	void createVertexBuffer(vkGroups::VertexBufferGroup&  group, std::vector<uint32_t>& verts);

	std::vector<std::vector<vk::PipelineShaderStageCreateInfo>> getShaderStages(vkGroups::PipelineShaderStageGroup vertexShader, vkGroups::PipelineShaderStageGroup fragmentShader);
};