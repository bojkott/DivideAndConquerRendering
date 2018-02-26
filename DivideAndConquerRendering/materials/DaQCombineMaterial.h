#pragma once
#include "../Material.h"

class DaQCombineMaterial : public Material
{
public:
	DaQCombineMaterial();
	vk::PipelineVertexInputStateCreateInfo getVertexinputInfo();

};