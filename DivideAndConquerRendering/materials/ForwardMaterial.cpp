#include "ForwardMaterial.h"

ForwardMaterial::ForwardMaterial() : Material("Forward.vert", "Forward.frag", DeviceContext::RENDERPASS_TYPE::Standard)
{
	vk::DescriptorSetLayoutBinding uboLayoutBinding;
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

	vk::DescriptorSetLayoutCreateInfo layoutInfo;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	vk::VertexInputBindingDescription bindingDescription;
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(float) * 4;
	bindingDescription.inputRate = vk::VertexInputRate::eVertex;
	bindingDescriptions.push_back(bindingDescription);

	bindingDescription.binding = 1;
	bindingDescription.stride = sizeof(float) * 3;
	bindingDescriptions.push_back(bindingDescription);

	bindingDescription.binding = 2;
	bindingDescription.stride = sizeof(float) * 4;
	bindingDescriptions.push_back(bindingDescription);

	VkVertexInputAttributeDescription attributeDescription = {};
	attributeDescription.binding = 0;
	attributeDescription.location = 0;
	attributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescription.offset = 0;
	attributeDescriptions.push_back(attributeDescription);

	attributeDescription.binding = 1;
	attributeDescription.location = 1;
	attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescription.offset = 0;
	attributeDescriptions.push_back(attributeDescription);

	attributeDescription.binding = 2;
	attributeDescription.location = 2;
	attributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescription.offset = 0;
	attributeDescriptions.push_back(attributeDescription);
}
