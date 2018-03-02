#include "DeviceGroup.h"
#include "DeviceContext.h"
#include "UniformBuffer.h"
#include "Mesh.h"
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

void DeviceGroup::createUniformBufferGroup(size_t bufferSize, vkGroups::UniformBufferGroup & bufferGroup)
{
	for (DeviceContext* const device : devices)
	{
		bufferGroup.sets.insert(std::make_pair(device, new UniformBuffer(device, bufferSize)));
	}
}

void DeviceGroup::createMeshGroup(std::vector<ModelHelper::MeshInfo> meshesInfo, Model* model, vkGroups::MeshGroup & meshGroup)
{
	for (DeviceContext* const device : devices)
	{
		meshGroup.sets[device];
		for (auto & meshInfo : meshesInfo)
		{
			Mesh* mesh = new Mesh(device, model, meshInfo);
			meshGroup.sets[device].push_back(mesh);
		}
	}
	
}

void DeviceGroup::createAdvancedMeshGroup(std::vector<ModelHelper::MeshInfo> meshesInfo, Model * model, vkGroups::AdvancedMeshGroup & advancedMeshGroup)
{

	for (auto & meshInfo : meshesInfo)
	{
		vkGroups::MeshGroup& meshGroup = advancedMeshGroup.sets[meshInfo.material];

		for (DeviceContext* const device : devices)
		{
			Mesh* mesh = new Mesh(device, model, meshInfo);
			meshGroup.sets[device].push_back(mesh);
		}
	}
}

