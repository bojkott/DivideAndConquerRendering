#pragma once

#include "RenderState.h"
#include "DeviceGroup.h"
#include "Material.h"
#include "Texture.h"

#include <vulkan\vulkan.hpp>
class UniformBuffer;
class Technique
{
private:

	vk::Pipeline pipeline;
	vk::PipelineCache pipelineCache;
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::DescriptorPool descriptorPool;
	vk::Sampler textureSampler;
	std::vector<vk::DescriptorSet> descriptorSets;
	vk::PipelineLayout pipelineLayout;
	std::map<int, Texture*> textures;

	Material* material = nullptr;
	UniformBuffer* materialBuffer = nullptr;
	RenderState* renderState = nullptr;
	DeviceContext* deviceContext;

	static std::map<Material*, vkGroups::TechniqueGroup> loadedTechniques;

public:
	Technique(DeviceContext* deviceContext, Material* m, RenderState* r);

	static Technique* createOrGetTechnique(DeviceContext* deviceContext, Material* m, RenderState* r);
	~Technique();

	void bindMaterial();
	void bindPipeline(vk::CommandBuffer& commandBuffer);
	void bindDescriptorSet(vk::CommandBuffer& commandBuffer);

	std::vector<vk::DescriptorSet> getDescriptionSets();

	vk::PipelineLayout& getPipelineLayout();

private:
	void createTexturesFromMaterial();
	void bindTextures();
	void createTextureSampler();
};
