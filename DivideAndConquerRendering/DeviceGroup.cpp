#include "DeviceGroup.h"
#include "DeviceContext.h"
#include "ModelHelper.h"

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



void DeviceGroup::createShaderGroup(const std::string & shaderFilename, Shader::ShaderType shaderType, vkGroups::ShaderGroup& shaderGroup)
{
	for (DeviceContext* const device : devices)
	{
		shaderGroup.sets.insert(std::make_pair(device, new Shader(shaderFilename, shaderType, device)));
	}
}

void DeviceGroup::createModelGroup(vkGroups::ModelGroup& modelGroup, const std::string & modelName, const std::string & textureName)
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	ModelHelper::loadModelFromFile(modelName, textureName, vertices, indices);

	for (DeviceContext* const device : devices)
	{
		modelGroup.sets[device] = Model(device, vertices, indices);
	}
}


vkGroups::BufferGroup DeviceGroup::createBuffer(vk::BufferCreateInfo bufferInfo, vk::Optional<const vk::AllocationCallbacks> allocator)
{
	vkGroups::BufferGroup group;
	for (DeviceContext* device : devices)
	{
		group.sets.insert(std::make_pair(device, device->getDevice().createBuffer(bufferInfo)));
	}
	return group;
}

vkGroups::BufferMemoryGroup DeviceGroup::allocateMemory(vkGroups::BufferGroup & bufferGroup, vk::MemoryPropertyFlags properies, vk::Optional<const vk::AllocationCallbacks> allocator)
{
	vkGroups::BufferMemoryGroup group;
	for (DeviceContext* device : devices)
	{
		vk::MemoryRequirements memRequirements = device->getDevice().getBufferMemoryRequirements(bufferGroup.sets[device]);
		vk::MemoryAllocateInfo allocInfo;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = device->findMemoryType(memRequirements.memoryTypeBits, properies);

		group.sets.insert(std::make_pair(device, device->getDevice().allocateMemory(allocInfo)));
	}
	return group;
}


void DeviceGroup::bindBufferMemory(vkGroups::BufferGroup & bufferGroup, vkGroups::BufferMemoryGroup & bufferMemoryGroup, vk::DeviceSize memoryOffest)
{
	for (DeviceContext* device : devices)
	{
		device->getDevice().bindBufferMemory(bufferGroup.sets[device], bufferMemoryGroup.sets[device], memoryOffest);
	}
}

void DeviceGroup::copyDataToGPUs(stbi_uc * bufferData, vkGroups::BufferMemoryGroup bufferMemoryGroup, vk::DeviceSize imageSize, vk::DeviceSize offset, vk::MemoryMapFlags flasg)
{
	for (DeviceContext* device : devices)
	{
		void* data;
		device->getDevice().mapMemory(bufferMemoryGroup.sets[device], offset, imageSize, flasg, &data);
		memcpy(data, bufferData, static_cast<size_t>(imageSize));
		device->getDevice().unmapMemory(bufferMemoryGroup.sets[device]);
	}
}
