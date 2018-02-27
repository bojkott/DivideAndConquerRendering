#include "Material.h"
#include "Renderer.h"


std::vector<Material*> Material::materials;
Material::Material(std::string vertexFilename, std::string fragmentFilename, DeviceContext::RENDERPASS_TYPE renderPassType)
{
	type = renderPassType;
	Renderer::deviceGroup.createShaderGroup("../assets/shaders/" + vertexFilename + ".spv", Shader::ShaderType::VS, vertexShader);
	Renderer::deviceGroup.createShaderGroup("../assets/shaders/" + fragmentFilename + ".spv", Shader::ShaderType::PS, fragmentShader);

	poolSizes.insert(std::make_pair(vk::DescriptorType::eUniformBuffer, 1)); //pool cant be empty :S
}

Material::~Material()
{
}

vkGroups::ShaderGroup Material::getVertexShader()
{
	return vertexShader;
}

vkGroups::ShaderGroup Material::getFragmentShader()
{
	return fragmentShader;
}

void Material::getShaderStages(DeviceContext * device, std::vector<vk::PipelineShaderStageCreateInfo>& stages)
{
	
	stages.push_back(vertexShader.sets.at(device)->getShaderStageInfo());
	stages.push_back(fragmentShader.sets.at(device)->getShaderStageInfo());
}

vk::DescriptorPoolCreateInfo Material::getDescriptorPoolInfo()
{
	poolSizesValues.clear();
	
	for (auto& poolSize : poolSizes)
	{
		vk::DescriptorPoolSize descriptor;
		descriptor.type = poolSize.first;
		descriptor.descriptorCount = poolSize.second;
		poolSizesValues.push_back(descriptor);
	}

	vk::DescriptorPoolCreateInfo poolInfo;
	poolInfo.poolSizeCount = poolSizesValues.size();
	poolInfo.pPoolSizes = poolSizesValues.data();

	poolInfo.maxSets = 1;

	return poolInfo;
}

vk::PipelineLayoutCreateInfo Material::getPipelineLayoutInfo()
{
	
	pushConstantsValues.clear();

	for (auto &pushConsant : pushConstants) 
	{
		vk::PushConstantRange range;
		range.offset = 0;
		range.stageFlags = pushConsant.first;
		range.size = pushConsant.second;
		pushConstantsValues.push_back(range);
	}

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.setLayoutCount = 1; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = pushConstantsValues.size(); 
	pipelineLayoutInfo.pPushConstantRanges = pushConstantsValues.data();

	return pipelineLayoutInfo;
}

vk::DescriptorSetLayoutCreateInfo Material::getDescriptorSetLayoutInfo()
{
	vk::DescriptorSetLayoutCreateInfo layoutInfo;
	layoutInfo.bindingCount = descriptorSetLayoutbindings.size();
	layoutInfo.pBindings = descriptorSetLayoutbindings.data();
	return layoutInfo;
}

void Material::addBinding(int binding, vk::DescriptorType type, vk::ShaderStageFlags stageFlags)
{
	vk::DescriptorSetLayoutBinding layoutBinding;

	layoutBinding.binding = binding;
	layoutBinding.descriptorCount = 1;
	layoutBinding.descriptorType = type;
	layoutBinding.pImmutableSamplers = nullptr;
	layoutBinding.stageFlags = stageFlags;
	descriptorSetLayoutbindings.push_back(layoutBinding);

	if (poolSizes.find(type) != poolSizes.end())
	{
		poolSizes[type]++;
	}
	else
		poolSizes.insert(std::make_pair(type, 1));
}

void Material::setPushConstant(vk::ShaderStageFlagBits shaderFlags, size_t size)
{
	if (pushConstants.find(shaderFlags) != pushConstants.end())
	{
		pushConstants[shaderFlags] = size;
	}
	else
		pushConstants.insert(std::make_pair(shaderFlags, size));
	
}

vk::PipelineVertexInputStateCreateInfo Material::getVertexinputInfo()
{
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	return vertexInputInfo;
}

DeviceContext::RENDERPASS_TYPE Material::getRenderPassType()
{
	return type;
}
