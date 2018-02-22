#pragma once
#include "../Material.h"

class DaQCombineMaterial : public Material
{
public:

	vk::PipelineVertexInputStateCreateInfo getVertexinputInfo();
	vk::DescriptorPoolCreateInfo getDescriptorPoolInfo();
	vk::PipelineLayoutCreateInfo getPipelineLayoutInfo();
	vk::DescriptorSetLayoutCreateInfo getDescriptorSetLayoutInfo();

};