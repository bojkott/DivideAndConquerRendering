#include "DeviceGroup.h"
#include "DeviceContext.h"
DeviceGroup::DeviceGroup()
{
}

DeviceGroup::~DeviceGroup()
{
	devices.clear();
}

void DeviceGroup::addDevice(vk::Instance & instance, vk::PhysicalDevice physicalDevice)
{
	devices.push_back(DeviceContext(instance, physicalDevice));
}

void DeviceGroup::addDevice(vk::Instance & instance, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR & surface)
{
	devices.push_back(DeviceContext(instance, physicalDevice, surface));
}

vkGroups::PipelineGroup DeviceGroup::createGraphicsPipeline(vk::PipelineCache pipelineCache, const vk::GraphicsPipelineCreateInfo & createInfo, vk::Optional<const vk::AllocationCallbacks> allocator = nullptr)
{
	vkGroups::PipelineGroup pipelineGroup;
	for (DeviceContext& device : devices)
	{
		pipelineGroup.sets.insert(std::make_pair(&device, device.getDevice().createGraphicsPipeline(pipelineCache, createInfo, allocator)));
	}
	return pipelineGroup;
}