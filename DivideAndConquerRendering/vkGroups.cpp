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


}
