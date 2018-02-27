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

	vk::Pipeline pipeline;
	vk::PipelineCache pipelineCache;
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::DescriptorPool descriptorPool;
	std::vector<vk::DescriptorSet> descriptorSets;
	vk::PipelineLayout pipelineLayout;


	Material* material = nullptr;
	RenderState* renderState = nullptr;


	DeviceContext* deviceContext;
public:
	Technique(DeviceContext* deviceContext, Material* m, RenderState* r);
	~Technique();

	void bind(vk::CommandBuffer& commandBuffer);
};
