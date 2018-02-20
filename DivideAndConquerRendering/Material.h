#pragma once
#include <vulkan\vulkan.hpp>
#include "vkGroups.h"
#include "DeviceGroup.h"

class Shader;

class Material
{
private:
	vkGroups::PipelineShaderStageGroup vertexShaders;
	vkGroups::PipelineShaderStageGroup fragmentShaders;
public:
	Material();
	~Material();
private:


};