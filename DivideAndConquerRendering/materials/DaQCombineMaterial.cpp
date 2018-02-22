#include "DaQCombineMaterial.h"

vk::PipelineVertexInputStateCreateInfo DaQCombineMaterial::getVertexinputInfo()
{
	std::vector<vk::VertexInputBindingDescription> bindingDescriptions;

	vk::VertexInputBindingDescription bindingDescription;
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(float) * 4;
	bindingDescription.inputRate = vk::VertexInputRate::eVertex;
	bindingDescriptions.push_back(bindingDescription);

	bindingDescription.binding = 1;
	bindingDescription.stride = sizeof(float) * 2;
	bindingDescriptions.push_back(bindingDescription);

	bindingDescription.binding = 2;
	bindingDescription.stride = sizeof(float) * 4;
	bindingDescriptions.push_back(bindingDescription);


	std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

	VkVertexInputAttributeDescription attributeDescription = {};
	attributeDescription.binding = 0;
	attributeDescription.location = 0;
	attributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescription.offset = 0;
	attributeDescriptions.push_back(attributeDescription);

	attributeDescription.binding = 1;
	attributeDescription.location = 1;
	attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescription.offset = 0;
	attributeDescriptions.push_back(attributeDescription);

	attributeDescription.binding = 2;
	attributeDescription.location = 2;
	attributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescription.offset = 0;
	attributeDescriptions.push_back(attributeDescription);


	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	return vertexInputInfo;
}

vk::DescriptorPoolCreateInfo DaQCombineMaterial::getDescriptorPoolInfo()
{
	std::vector<vk::DescriptorPoolSize> poolSizes;
	vk::DescriptorPoolSize poolSize;
	poolSize.type = vk::DescriptorType::eUniformBuffer;
	poolSize.descriptorCount = 2;
	poolSizes.push_back(poolSize);
	poolSize.type = vk::DescriptorType::eCombinedImageSampler;
	poolSize.descriptorCount = 1;
	poolSizes.push_back(poolSize);

	vk::DescriptorPoolCreateInfo poolInfo;
	poolInfo.poolSizeCount = poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();

	poolInfo.maxSets = 1;

	return poolInfo;
}

vk::PipelineLayoutCreateInfo DaQCombineMaterial::getPipelineLayoutInfo()
{
	std::vector<vk::PushConstantRange> pushConstants;

	vk::PushConstantRange pushConstantRange = {};
	pushConstantRange.size = sizeof(float) * 4; //float4
	pushConstantRange.offset = 0;
	pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
	pushConstants.push_back(pushConstantRange);
	pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eFragment;
	pushConstantRange.offset = sizeof(float) * 4;
	pushConstants.push_back(pushConstantRange);

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.setLayoutCount = 1; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = pushConstants.size(); // Optional
	pipelineLayoutInfo.pPushConstantRanges = pushConstants.data(); // Optional
	return pipelineLayoutInfo;
}

vk::DescriptorSetLayoutCreateInfo DaQCombineMaterial::getDescriptorSetLayoutInfo()
{
	std::vector<vk::DescriptorSetLayoutBinding> bindings;

	vk::DescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.descriptorCount = 1;

	//uboLayoutBinding.binding = POSITION;
	//uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	//uboLayoutBinding.descriptorCount = 1;
	//uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	//uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	//bindings.push_back(uboLayoutBinding);

	//uboLayoutBinding.binding = NORMAL;
	//uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	//uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
	//bindings.push_back(uboLayoutBinding);

	//uboLayoutBinding.binding = TEXTCOORD;
	//uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	//uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
	//bindings.push_back(uboLayoutBinding);

	uboLayoutBinding.binding = 3;
	uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
	uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
	bindings.push_back(uboLayoutBinding);

	uboLayoutBinding.binding = 4;
	uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
	uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eAll;
	bindings.push_back(uboLayoutBinding);

	//texture
	uboLayoutBinding.binding = 5;
	uboLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
	uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
	bindings.push_back(uboLayoutBinding);

	vk::DescriptorSetLayoutCreateInfo layoutInfo;
	layoutInfo.bindingCount = bindings.size();
	layoutInfo.pBindings = bindings.data();

	return layoutInfo;
}