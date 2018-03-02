#pragma once

#include "RenderState.h"
#include "DeviceGroup.h"
#include "Material.h"

#include <vulkan\vulkan.hpp>
class UniformBuffer;
class Technique
{
private:

	vk::Pipeline pipeline;
	vk::PipelineCache pipelineCache;
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::DescriptorPool descriptorPool;
	std::vector<vk::DescriptorSet> descriptorSets;
	vk::PipelineLayout pipelineLayout;


	Material* material = nullptr;
	UniformBuffer* materialBuffer = nullptr;
	RenderState* renderState = nullptr;
	DeviceContext* deviceContext;

	static std::map<Material*, vkGroups::TechniqueGroup> loadedTechniques;

public:
	Technique(DeviceContext* deviceContext, Material* m, RenderState* r);

	static Technique* createOrGetTechnique(DeviceContext* deviceContext, Material* m, RenderState* r);
	~Technique();

	void bind(vk::CommandBuffer& commandBuffer);

	std::vector<vk::DescriptorSet> getDescriptionSets();

	vk::PipelineLayout& getPipelineLayout();
};
