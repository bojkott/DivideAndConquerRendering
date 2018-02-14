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


	ShaderModuleGroup::~ShaderModuleGroup()
	{
		for (auto const& set : sets)
		{
			set.first->getDevice().destroyShaderModule(set.second);
		}
	}

}
