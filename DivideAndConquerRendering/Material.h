#pragma once
#include <vulkan\vulkan.hpp>
#include "vkGroups.h"
#include "DeviceGroup.h"

class Shader;

class Material
{
public:
	enum class ShaderType { VS = 0, PS = 1, GS = 2, CS = 3 };
private:
	vkGroups::PipelineShaderStageGroup vertexShader;
	vkGroups::PipelineShaderStageGroup fragmentShader;
public:
	Material(std::string& vertexFilename, std::string& fragmentFilename);
	~Material();

	std::vector<vk::PipelineShaderStageCreateInfo> getShaderStages();
private:


};