#pragma once
#include "Common.h"
#include "DeviceGroup.h"
#include <fstream>
#include <vulkan/vulkan.hpp>

class Shader
{
private:

public:
	enum class Type { VERTEX = 0x00000001, FRAGMENT = 0x00000010 };
	static std::vector<vk::PipelineShaderStageCreateInfo> createPipelineShaderStage(
		const std::string& vertexFilename, const std::string& fragmentFilename,
		const vk::Device& device);

	
private:
	Shader();
	static std::vector<char> readFile(const std::string& filename);

	static vk::PipelineShaderStageCreateInfo createShaderModule(const std::string& filename,
		Type shaderType, const vk::Device& device);

};