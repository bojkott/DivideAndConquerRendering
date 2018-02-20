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

std::vector<vk::PipelineShaderStageCreateInfo> Material::getShaderStages()
{
	std::vector<vk::PipelineShaderStageCreateInfo> vec;
	vk::PipelineShaderStageCreateInfo shaderStages[2];
	for (int i = 0; i < vertexShader.sets.size(); ++i)
	{
		//shaderStages[0] = vertexShader.sets[i]

	}
	return vec;
}
