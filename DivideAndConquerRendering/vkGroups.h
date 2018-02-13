#pragma once
#include <map>
#include <vulkan\vulkan.hpp>

class DeviceContext;
namespace vkGroups {
	struct PipelineGroup {
		std::map<DeviceContext*, vk::Pipeline> sets;
		~PipelineGroup();
	};
	struct DescriptorPoolGroup {
		std::map<DeviceContext*, vk::DescriptorPool> sets;
		~DescriptorPoolGroup();
	};

}