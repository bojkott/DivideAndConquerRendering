#include "DaQCombineMaterial.h"

DaQCombineMaterial::DaQCombineMaterial() : Material("DaQCombine.vert", "DaQCombine.frag")
{
	addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment);
}

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