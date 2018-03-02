#include "ForwardMaterial.h"
#include "../ModelHelper.h"

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
	bindingDescription.stride = sizeof(ModelHelper::Vertex);
	bindingDescription.inputRate = vk::VertexInputRate::eVertex;
	bindingDescriptions.push_back(bindingDescription);

	VkVertexInputAttributeDescription attributeDescription = {};
	attributeDescription.binding = 0;
	attributeDescription.location = 0;
	attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;	//Pos
	attributeDescription.offset = offsetof(ModelHelper::Vertex, ModelHelper::Vertex::pos);
	attributeDescriptions.push_back(attributeDescription);

	attributeDescription.binding = 0;
	attributeDescription.location = 1;
	attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;	//Color
	attributeDescription.offset = offsetof(ModelHelper::Vertex, ModelHelper::Vertex::color);
	attributeDescriptions.push_back(attributeDescription);

	attributeDescription.binding = 0;
	attributeDescription.location = 2;
	attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;	//TexCoord
	attributeDescription.offset = offsetof(ModelHelper::Vertex, ModelHelper::Vertex::texCoord);
	attributeDescriptions.push_back(attributeDescription);

	//attributeDescription.binding = 0;
	//attributeDescription.location = 3;
	//attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;	//Norm (When added)
	//attributeDescription.offset = offsetof(ModelHelper::Vertex, ModelHelper::Vertex::norm);
	//attributeDescriptions.push_back(attributeDescription);
	
}
