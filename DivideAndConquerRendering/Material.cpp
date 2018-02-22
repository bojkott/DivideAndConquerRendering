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

