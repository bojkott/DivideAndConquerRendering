#include "vkGroups.h"
#include "DeviceContext.h"

namespace vkGroups {
	PipelineGroup::~PipelineGroup()
	{
		for (auto const& set : sets)
		{
			set.first->getDevice().destroyPipeline(set.second);
		}
	}


	DescriptorPoolGroup::~DescriptorPoolGroup()
	{
		for (auto const& set : sets)
		{
			set.first->getDevice().destroyDescriptorPool(set.second);
		}
	}

	DescriptorSetGroup::~DescriptorSetGroup()
	{
		//No destroy
	}

	PipelineLayoutGroup::~PipelineLayoutGroup()
	{
		for (auto const& set : sets)
		{
			set.first->getDevice().destroyPipelineLayout(set.second);
		}
	}


	PipelineShaderStageGroup::~PipelineShaderStageGroup()
	{
		for (auto const& set : sets)
		{
			set.first->getDevice().destroyShaderModule(set.second.module);
		}
	}

	GraphicsPipelineCreateInfoGroup::~GraphicsPipelineCreateInfoGroup()
	{
		for (auto const& set : sets)
		{
			//set.first->getDevice().destroyPipeline(set.second.);
		}
	}

	PipelineCacheGroup::~PipelineCacheGroup()
	{
		for (auto const& set : sets)
		{
			set.first->getDevice().destroyPipelineCache(set.second);
		}
	}
	DescriptorSetLayoutGroup::~DescriptorSetLayoutGroup()
	{
		for (auto const& set : sets)
		{
			set.first->getDevice().destroyDescriptorSetLayout(set.second);
		}
	}

	VertexBufferGroup::~VertexBufferGroup()
	{
		for (auto& set : sets)
		{
			set.second.Destroy();
		}
	}
}
