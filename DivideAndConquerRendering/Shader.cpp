#include "Shader.h"
#include "Window.h"
#include "Renderer.h"

vk::PipelineShaderStageCreateInfo Shader::createPipelineShaderStage(const std::string & shaderFilename, Type shaderType, const vk::Device& device)
{
	vk::PipelineShaderStageCreateInfo shaderStage;
	shaderStage = createShaderModule(shaderFilename, shaderType, device);
	return shaderStage;
}

std::vector<char> Shader::readFile(const std::string & filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		throw std::runtime_error("failed to open shader file");

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

vk::PipelineShaderStageCreateInfo Shader::createShaderModule(const std::string & filename, Type shaderType,
	 const vk::Device& device)
{
	std::vector<char> shaderCode = readFile(filename);
	vk::ShaderModuleCreateInfo createInfo = {};
	createInfo.codeSize = shaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*> (shaderCode.data());

	vk::ShaderModule shaderModule; 
	if (FAILED(device.createShaderModule(&createInfo, nullptr, &shaderModule)))
	{
		throw std::runtime_error("Failed to create shader module");
	}

	
	vk::PipelineShaderStageCreateInfo shaderStageInfo = {};
	shaderStageInfo.stage = static_cast<vk::ShaderStageFlagBits> (shaderType);
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = "main";


	return shaderStageInfo;
}
