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

}
