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



void DeviceGroup::createShaderGroup(const std::string & shaderFilename, Shader::ShaderType shaderType, vkGroups::ShaderGroup& shaderGroup)
{
	for (DeviceContext* const device : devices)
	{
		shaderGroup.sets.insert(std::make_pair(device, new Shader(shaderFilename, shaderType, device)));
	}
}

