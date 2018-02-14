#pragma once

#include "RenderState.h"
#include "DeviceGroup.h"

#include <vulkan\vulkan.hpp>

class Technique
{
public:
	int id;
private:
	vk::Pipeline pipeline;
	vk::DescriptorSetLayout descriptorSetLayout;
	vkGroups::DescriptorPoolGroup descriptorPoolGroup;
	vkGroups::DescriptorSetGroup descriptorSetGroup;
	vkGroups::PipelineLayoutGroup pipelineLayoutGroup;

	//Material* material = nullptr;
	RenderState* renderState = nullptr;

	static int numberOfTechniques;
public:
	Technique(/*Material* m, */ RenderState* r);
	~Technique();

private:
	void createDescriptorPool();
	void createDescriptorSet();
	void createPipelineLayout();
};
