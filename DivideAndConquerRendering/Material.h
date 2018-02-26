#pragma once
#include <vulkan\vulkan.hpp>
#include "vkGroups.h"
#include "DeviceGroup.h"

class Shader;

class Material
{
public:
	
private:
	vkGroups::ShaderGroup vertexShader;
	vkGroups::ShaderGroup fragmentShader;

	std::vector<vk::DescriptorSetLayoutBinding> descriptorSetLayoutbindings;
	std::map<vk::DescriptorType, int> poolSizes;
	std::vector<vk::DescriptorPoolSize> poolSizesValues;
	std::map<vk::ShaderStageFlagBits, size_t> pushConstants;
	std::vector<vk::PushConstantRange> pushConstantsValues;


	static std::vector<Material*> materials;

public:
	Material(std::string vertexFilename, std::string fragmentFilename);
	~Material();

	vkGroups::ShaderGroup getVertexShader();
	vkGroups::ShaderGroup getFragmentShader();

	vk::DescriptorPoolCreateInfo getDescriptorPoolInfo();
	vk::PipelineLayoutCreateInfo getPipelineLayoutInfo();
	vk::DescriptorSetLayoutCreateInfo getDescriptorSetLayoutInfo();
	vk::PipelineVertexInputStateCreateInfo getVertexinputInfo();
	template<typename T>
	static T* getMaterial();

	template<typename T>
	static T* addMaterial();

protected:
	void addBinding(int binding, vk::DescriptorType type, vk::ShaderStageFlags stageFlags);
	void setPushConstant(vk::ShaderStageFlagBits shaderFlag, size_t size);

	std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
	std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
};

template<typename T>
static inline T * Material::getMaterial()
{
	for (Material* material : materials)
	{
		T* instance = dynamic_cast<T*>(material);
		if (instance != nullptr)
			return instance;
		else
			throw std::runtime_error("failed to find material");

	}
}

template<typename T>
static inline T * Material::addMaterial()
{
	T* mat = new T();
	materials.push_back(mat);
	return mat;
}
