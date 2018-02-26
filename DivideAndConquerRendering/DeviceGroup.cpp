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

void DeviceGroup::initDevices()
{
	for (DeviceContext* device : devices)
		device->initDevice();
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

void DeviceGroup::createGraphicsPipeline(vk::PipelineCache pipelineCache, const vk::GraphicsPipelineCreateInfo & createInfo, vkGroups::PipelineGroup& pipelineGroup, vk::Optional<const vk::AllocationCallbacks> allocator)
{
	for (DeviceContext* device : devices)
	{
		pipelineGroup.sets.insert(std::make_pair(device, device->getDevice().createGraphicsPipeline(pipelineCache, createInfo, allocator)));
	}
}

void DeviceGroup::createDescriptorPool(const vk::DescriptorPoolCreateInfo & poolInfo, vkGroups::DescriptorPoolGroup& descriptorPoolGroup, vk::Optional<const vk::AllocationCallbacks> allocator)
{
	
	for (DeviceContext* device : devices)
	{
		descriptorPoolGroup.sets.insert(std::make_pair(device, device->getDevice().createDescriptorPool(poolInfo, allocator)));
	}
}

void DeviceGroup::createDescriptorSetLayout(const vk::DescriptorSetLayoutCreateInfo& createInfo, vkGroups::DescriptorSetLayoutGroup& createDescriptorSetLayoutGroup, vk::Optional<const vk::AllocationCallbacks> allocator)
{
	for (DeviceContext* device : devices)
	{
		createDescriptorSetLayoutGroup.sets.insert(std::make_pair(device, device->getDevice().createDescriptorSetLayout(createInfo, allocator)));
	}
}

void DeviceGroup::allocateDescriptorSet(const vkGroups::DescriptorPoolGroup& descriptorPool, const vkGroups::DescriptorSetLayoutGroup& descriptorSetLayout, vkGroups::DescriptorSetGroup& descriptorSetGroup)
{
	for (DeviceContext* device : devices)
	{
		vk::DescriptorSet descriptorSet;
		vk::DescriptorSetLayout layouts[] = { descriptorSetLayout.sets.at(device) };
		vk::DescriptorSetAllocateInfo allocInfo = {};
		allocInfo.descriptorPool = descriptorPool.sets.at(device);
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = layouts;
		device->getDevice().allocateDescriptorSets(&allocInfo, &descriptorSet);
		descriptorSetGroup.sets.insert(std::make_pair(device, descriptorSet));	//If this fail it could be due to that descriptorSet is defined in the loop and go out of scope. How this works needs to be reworked.
	}
}

void DeviceGroup::createPipelineLayout(vk::PipelineLayoutCreateInfo& pipelineLayoutInfo, const vkGroups::DescriptorSetLayoutGroup& descriptorSetLayout, vkGroups::PipelineLayoutGroup& layoutGroup, vk::Optional<const vk::AllocationCallbacks> allocator)
{
	pipelineLayoutInfo.setLayoutCount = 1;
	

	for (DeviceContext* device : devices)
	{
		vk::DescriptorSetLayout layouts[] = { descriptorSetLayout.sets.at(device) };
		pipelineLayoutInfo.pSetLayouts = layouts;
		layoutGroup.sets.insert(std::make_pair(device, device->getDevice().createPipelineLayout(pipelineLayoutInfo, allocator)));
	}
}

void DeviceGroup::createShaderGroup(const std::string & shaderFilename, Shader::ShaderType shaderType, vkGroups::ShaderGroup& shaderGroup)
{
	for (DeviceContext* const device : devices)
	{
		shaderGroup.sets.insert(std::make_pair(device, new Shader(shaderFilename, shaderType, device)));
	}
}

void DeviceGroup::createPipelineCache(vkGroups::PipelineCacheGroup& group)
{
	for (DeviceContext* device : devices)
	{
		group.sets.insert(std::make_pair(device, device->getDevice().createPipelineCache(vk::PipelineCacheCreateInfo())));
	}
}

void DeviceGroup::createPipeline(vk::GraphicsPipelineCreateInfo & pipelineInfo,
	vkGroups::PipelineCacheGroup pipelineCacheGroup,
	vkGroups::ShaderGroup vertexShader,
	vkGroups::ShaderGroup fragmentShader,
	vkGroups::PipelineLayoutGroup pipelineLayoutGroup,
	vkGroups::PipelineGroup& group,
	vk::Optional<const vk::AllocationCallbacks> allocator)
{
	for (DeviceContext* device : devices)
	{
		vk::PipelineShaderStageCreateInfo stages[2] = { vertexShader.sets.at(device)->getShaderStageInfo(), fragmentShader.sets.at(device)->getShaderStageInfo() };	//This only supports vertex- and fragmentshader.
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = stages;
		pipelineInfo.layout = pipelineLayoutGroup.sets.at(device);
		pipelineInfo.renderPass = device->getRenderpass();
		group.sets.insert(std::make_pair(device, device->getDevice().createGraphicsPipeline(pipelineCacheGroup.sets.at(device), pipelineInfo)));
	}
}


