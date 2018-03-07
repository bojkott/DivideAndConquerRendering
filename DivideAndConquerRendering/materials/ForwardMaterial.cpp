#include "ForwardMaterial.h"
#include "../ModelHelper.h"

ForwardMaterial::ForwardMaterial(tinyobj::material_t material) : Material("Forward.vert", "Forward.frag", DeviceContext::RENDERPASS_TYPE::Standard)
{
	this->material = material;
	
	if (this->material.ambient_texname != "")
		addBinding(PER_MATERIAL_BINDING, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment);
	if (this->material.diffuse_texname != "")
		addBinding(PER_MATERIAL_BINDING, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment);
	if (this->material.specular_texname != "")
		addBinding(PER_MATERIAL_BINDING, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment);
	if (this->material.specular_highlight_texname != "")
		addBinding(PER_MATERIAL_BINDING, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment);
	if (this->material.bump_texname != "")
		addBinding(PER_MATERIAL_BINDING, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment);
	if (this->material.displacement_texname != "")
		addBinding(PER_MATERIAL_BINDING, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment);
	if (this->material.alpha_texname != "")
		addBinding(PER_MATERIAL_BINDING, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment);
	if (this->material.reflection_texname != "")
		addBinding(PER_MATERIAL_BINDING, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment);

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
	attributeDescription.format = vk::Format::eR32G32B32A32Sfloat;	//Pos
	attributeDescription.offset = offsetof(ModelHelper::Vertex, pos);
	attributeDescriptions.push_back(attributeDescription);


	attributeDescription.binding = 0;
	attributeDescription.location = 1;
	attributeDescription.format = vk::Format::eR32G32Sfloat;	//TexCoord
	attributeDescription.offset = offsetof(ModelHelper::Vertex, texCoord);
	attributeDescriptions.push_back(attributeDescription);

	attributeDescription.binding = 0;
	attributeDescription.location = 2;
	attributeDescription.format = vk::Format::eR32G32B32A32Sfloat;	//Norm
	attributeDescription.offset = offsetof(ModelHelper::Vertex, norm);
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

std::string ForwardMaterial::getAmbientTexname()
{
	return this->material.ambient_texname;
}

std::string ForwardMaterial::getDiffuseTexname()
{
	return this->material.diffuse_texname;
}

std::string ForwardMaterial::getSpecularTexname()
{
	return this->material.specular_texname;
}

std::string ForwardMaterial::getSpecularHighlightTexname()
{
	return this->material.specular_highlight_texname;
}

std::string ForwardMaterial::getBumpTexname()
{
	return this->material.bump_texname;
}

std::string ForwardMaterial::getDisplacementTexname()
{
	return this->material.displacement_texname;
}

std::string ForwardMaterial::getAlphaTexname()
{
	return this->material.alpha_texname;
}

std::string ForwardMaterial::getReflectionTexname()
{
	return this->material.reflection_texname;
}

void * ForwardMaterial::getMaterialBufferData()
{
	return &buffer;
}
