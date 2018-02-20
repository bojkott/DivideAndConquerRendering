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

