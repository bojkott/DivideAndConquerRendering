#include "ForwardMaterial.h"
#include "../ModelHelper.h"

ForwardMaterial::ForwardMaterial(tinyobj::material_t material) : Material("Forward.vert", "Forward.frag", DeviceContext::RENDERPASS_TYPE::Standard)
{
	this->material = material;

	addBinding(PER_CAMERA_BINDING, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex);
	addBinding(PER_MATERIAL_BINDING, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eAllGraphics);
	setPushConstant(vk::ShaderStageFlagBits::eVertex, sizeof(glm::mat4));	//Per_Object

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
	attributeDescription.format = vk::Format::eR32G32Sfloat;	//TexCoord
	attributeDescription.offset = offsetof(ModelHelper::Vertex, ModelHelper::Vertex::texCoord);
	attributeDescriptions.push_back(attributeDescription);

	attributeDescription.binding = 0;
	attributeDescription.location = 2;
	attributeDescription.format = vk::Format::eR32G32B32Sfloat;	//Norm
	attributeDescription.offset = offsetof(ModelHelper::Vertex, ModelHelper::Vertex::norm);
	attributeDescriptions.push_back(attributeDescription);


	buffer.ambient = (glm::vec3&)material.ambient;
	buffer.objColor = (glm::vec3&)material.diffuse;

}

tinyobj::material_t* ForwardMaterial::getMaterial()
{
	return &material;
}

size_t ForwardMaterial::getMaterialBufferSize()
{
	return sizeof(MaterialBuffer);
}

void * ForwardMaterial::getMaterialBufferData()
{
	return &buffer;
}
