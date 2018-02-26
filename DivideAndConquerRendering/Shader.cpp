#include "Shader.h"
#include "Window.h"
#include "Renderer.h"

Shader::Shader(const std::string & shaderFilename, ShaderType shaderType, DeviceContext * deviceContext): shaderType(shaderType), deviceContext(deviceContext)
{
	createShaderModule(shaderFilename);
}

Shader::~Shader()
{
	deviceContext->getDevice().destroyShaderModule(shaderModule);
}

vk::PipelineShaderStageCreateInfo Shader::getShaderStageInfo()
{
	vk::PipelineShaderStageCreateInfo shaderStageInfo = {};
	shaderStageInfo.stage = (vk::ShaderStageFlagBits)shaderType;
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = "main";


	return shaderStageInfo;
}

std::vector<char> Shader::readFile(const std::string & filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.good())
		throw std::runtime_error("failed to open shader file: " + filename);

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

void Shader::createShaderModule(const std::string & filename)
{
	std::vector<char> shaderCode = readFile(filename);
	vk::ShaderModuleCreateInfo createInfo = {};
	createInfo.codeSize = shaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*> (shaderCode.data());


	shaderModule = deviceContext->getDevice().createShaderModule(createInfo);
	
}
