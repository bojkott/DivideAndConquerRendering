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

	void createUniformBufferGroup(size_t bufferSize, vkGroups::UniformBufferGroup& bufferGroup);

	void createMeshGroup(std::vector<ModelHelper::MeshInfo> meshesInfo, Model* model, vkGroups::MeshGroup& meshGroup);

	void createAdvancedMeshGroup(std::vector<ModelHelper::MeshInfo> meshesInfo, Model* model, vkGroups::AdvancedMeshGroup& advancedMeshGroup);
};