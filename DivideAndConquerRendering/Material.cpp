#include "Material.h"
#include "Renderer.h"

Material::Material(std::string& vertexFilename, std::string& fragmentFilename)
{
	vertexShader = Renderer::deviceGroup.createPipelineShaderStage(vertexFilename, Shader::Type::VERTEX);
	fragmentShader = Renderer::deviceGroup.createPipelineShaderStage(fragmentFilename, Shader::Type::FRAGMENT);
}

Material::~Material()
{
}

vkGroups::PipelineShaderStageGroup Material::getVertexShader()
{
	return vertexShader;
}

vkGroups::PipelineShaderStageGroup Material::getFragmentShader()
{
	return fragmentShader;
}

std::vector<std::vector<vk::PipelineShaderStageCreateInfo>> Material::getShaderStages()
{
	return Renderer::deviceGroup.getShaderStages(vertexShader, fragmentShader);
}

std::vector<vk::VertexInputBindingDescription> Material::getBindingDescriptions()
{
	//Fetch information from somewhere. Like shader or where they are loaded.
	//This is dependent on what is set in AttributeDescriptions.
	return std::vector<vk::VertexInputBindingDescription>();
}

std::vector<vk::VertexInputAttributeDescription> Material::getAttributeDescriptions()
{
	//Fetch information from somewhere. Like shader or where they are loaded.
	return std::vector<vk::VertexInputAttributeDescription>();
}
