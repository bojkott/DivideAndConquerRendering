#pragma once

#include "RenderState.h"
#include "DeviceGroup.h"
#include "Material.h"

#include <vulkan\vulkan.hpp>

class Technique
{
public:
	int id;
private:
	vkGroups::PipelineGroup pipelineGroup;
	vkGroups::PipelineCacheGroup pipelineCacheGroup;
	vkGroups::DescriptorSetLayoutGroup descriptorSetLayoutGroup;
	vkGroups::DescriptorPoolGroup descriptorPoolGroup;
	vkGroups::DescriptorSetGroup descriptorSetGroup;
	vkGroups::PipelineLayoutGroup pipelineLayoutGroup;

	Material* material = nullptr;
	RenderState* renderState = nullptr;

	static int numberOfTechniques;
public:
	Technique(Material* m, RenderState* r);
	~Technique();

	vk::Pipeline getPipeline(DeviceContext* device);
};
