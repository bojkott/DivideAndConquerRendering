#include "DeviceGroup.h"
#include "DeviceContext.h"
DeviceGroup::DeviceGroup()
{
}

DeviceGroup::~DeviceGroup()
{
	for (DeviceContext* device : devices)
		delete device;
	devices.clear();
}

void DeviceGroup::addDevice(vk::Instance & instance, vk::PhysicalDevice physicalDevice)
{
	devices.push_back(new DeviceContext(this, instance, physicalDevice));
}

void DeviceGroup::addDevice(vk::Instance & instance, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR & surface)
{
	devices.push_back(new DeviceContext(this, instance, physicalDevice, surface));
}

unsigned int DeviceGroup::getGroupSize()
{
	return devices.size();
}

DeviceContext * DeviceGroup::getMainDevice()
{
	return devices[0];
}

std::vector<DeviceContext*> DeviceGroup::getDevices()
{
	return devices;
}

vkGroups::PipelineGroup DeviceGroup::createGraphicsPipeline(vk::PipelineCache pipelineCache, const vk::GraphicsPipelineCreateInfo & createInfo, vk::Optional<const vk::AllocationCallbacks> allocator)
{
	vkGroups::PipelineGroup pipelineGroup;
	for (DeviceContext* device : devices)
	{
		pipelineGroup.sets.insert(std::make_pair(device, device->getDevice().createGraphicsPipeline(pipelineCache, createInfo, allocator)));
	}
	return pipelineGroup;
}

vkGroups::DescriptorPoolGroup DeviceGroup::createDescriptorPool(const vk::DescriptorPoolCreateInfo & poolInfo, vk::Optional<const vk::AllocationCallbacks> allocator)
{
	vkGroups::DescriptorPoolGroup descriptorPoolGroup;
	for (DeviceContext* device : devices)
	{
		descriptorPoolGroup.sets.insert(std::make_pair(device, device->getDevice().createDescriptorPool(poolInfo, allocator)));
	}
	return descriptorPoolGroup;
}

vkGroups::DescriptorSetGroup DeviceGroup::allocateDescriptorSet(const vkGroups::DescriptorPoolGroup & descriptorPool, const vk::DescriptorSetLayout descriptorSetLayout)
{
	vkGroups::DescriptorSetGroup descriptorSetGroup;
	for (DeviceContext* device : devices)
	{
		vk::DescriptorSet descriptorSet;
		vk::DescriptorSetLayout layouts[] = { descriptorSetLayout };
		vk::DescriptorSetAllocateInfo allocInfo = {};
		allocInfo.descriptorPool = descriptorPool.sets.at(device);
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = layouts;
		device->getDevice().allocateDescriptorSets(&allocInfo, &descriptorSet);
		descriptorSetGroup.sets.insert(std::make_pair(device, descriptorSet));	//If this fail it could be due to that descriptorSet is defined in the loop and go out of scope. How this works needs to be reworked.
	}
	return descriptorSetGroup;
}

vkGroups::PipelineLayoutGroup DeviceGroup::createPipelineLayout(const vk::PipelineLayoutCreateInfo & pipelineLayoutInfo, vk::Optional<const vk::AllocationCallbacks> allocator)
{
	vkGroups::PipelineLayoutGroup pipelineLayoutGroup;
	for (DeviceContext* device : devices)
	{
		pipelineLayoutGroup.sets.insert(std::make_pair(device, device->getDevice().createPipelineLayout(pipelineLayoutInfo, allocator)));
	}
	return pipelineLayoutGroup;
}

vkGroups::PipelineShaderStageGroup DeviceGroup::createPipelineShaderStage(const std::string& shaderFilename, Shader::Type shaderType)
{
	vkGroups::PipelineShaderStageGroup pipelineShaderStageGroup;
	for (DeviceContext* const device : devices)
	{
		pipelineShaderStageGroup.sets.insert(std::make_pair(device, Shader::createPipelineShaderStage(shaderFilename, shaderType, device->getDevice())));
	}
	return pipelineShaderStageGroup;
}

vkGroups::PipelineCacheGroup DeviceGroup::createPipelineCache()
{
	vkGroups::PipelineCacheGroup group;
	for (DeviceContext* device : devices)
	{
		group.sets.insert(std::make_pair(device, device->getDevice().createPipelineCache(vk::PipelineCacheCreateInfo())));
	}
	return group;
}

vkGroups::PipelineGroup DeviceGroup::createPipeline(vk::GraphicsPipelineCreateInfo & pipelineInfo,
	vkGroups::PipelineCacheGroup pipelineCacheGroup,
	vkGroups::PipelineShaderStageGroup vertexShader,
	vkGroups::PipelineShaderStageGroup fragmentShader,
	vkGroups::PipelineLayoutGroup pipelineLayoutGroup,
	vk::Optional<const vk::AllocationCallbacks> allocator)
{
	vkGroups::PipelineGroup group;
	for (DeviceContext* device : devices)
	{
		vk::PipelineShaderStageCreateInfo stages[2] = { vertexShader.sets.at(device), fragmentShader.sets.at(device) };	//This only supports vertex- and fragmentshader.
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = stages;
		pipelineInfo.layout = pipelineLayoutGroup.sets.at(device);
		pipelineInfo.renderPass = device->getRenderpass();
		group.sets.insert(std::make_pair(device, device->getDevice().createGraphicsPipeline(pipelineCacheGroup.sets.at(device), pipelineInfo)));
	}
	return group;
}


std::vector<std::vector<vk::PipelineShaderStageCreateInfo>> DeviceGroup::getShaderStages(vkGroups::PipelineShaderStageGroup vertexShader, vkGroups::PipelineShaderStageGroup fragmentShader)
{
	std::vector<std::vector<vk::PipelineShaderStageCreateInfo>> vec;
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
	for (DeviceContext* const device : devices)
	{
		shaderStages.push_back(vertexShader.sets[device]);
		shaderStages.push_back(fragmentShader.sets[device]);
		vec.push_back(shaderStages);
	}
	return vec;
}
