#pragma once

#include "RenderState.h"

#include <vulkan\vulkan.hpp>

class Technique
{
public:
	int id;
private:
	vk::Pipeline pipeline;
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::DescriptorPool descriptorPool;

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
