#include "ForwardMaterial.h"
#include "../ModelHelper.h"

ForwardMaterial::ForwardMaterial(tinyobj::material_t material) : Material("Forward.vert", "Forward.frag", DeviceContext::RENDERPASS_TYPE::Standard)
{
	this->material = material;

	/*vk::DescriptorSetLayoutBinding uboLayoutBinding;
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

	vk::DescriptorSetLayoutCreateInfo layoutInfo;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;*/


	


	vk::VertexInputBindingDescription bindingDescription;
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(ModelHelper::Vertex);
	bindingDescription.inputRate = vk::VertexInputRate::eVertex;
	bindingDescriptions.push_back(bindingDescription);

	vk::VertexInputAttributeDescription attributeDescription;
	attributeDescription.binding = 0;
	attributeDescription.location = 0;
	attributeDescription.format = vk::Format::eR32G32B32Sfloat;	//Pos
	attributeDescription.offset = offsetof(ModelHelper::Vertex, ModelHelper::Vertex::pos);
	attributeDescriptions.push_back(attributeDescription);

	attributeDescription.binding = 0;
	attributeDescription.location = 1;
	attributeDescription.format = vk::Format::eR32G32B32Sfloat;	//Color
	attributeDescription.offset = offsetof(ModelHelper::Vertex, ModelHelper::Vertex::color);
	attributeDescriptions.push_back(attributeDescription);

	attributeDescription.binding = 0;
	attributeDescription.location = 2;
	attributeDescription.format = vk::Format::eR32G32Sfloat;	//TexCoord
	attributeDescription.offset = offsetof(ModelHelper::Vertex, ModelHelper::Vertex::texCoord);
	attributeDescriptions.push_back(attributeDescription);

	attributeDescription.binding = 0;
	attributeDescription.location = 3;
	attributeDescription.format = vk::Format::eR32G32B32Sfloat;	//Norm
	attributeDescription.offset = offsetof(ModelHelper::Vertex, ModelHelper::Vertex::norm);
	attributeDescriptions.push_back(attributeDescription);
	
}

tinyobj::material_t* ForwardMaterial::getMaterial()
{
	return &material;
}
