#pragma once
#include <map>
#include <vulkan\vulkan.hpp>

class Shader;
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

	struct DescriptorSetLayoutGroup {
		std::map<DeviceContext*, vk::DescriptorSetLayout> sets;
		~DescriptorSetLayoutGroup();
	};

	struct DescriptorSetGroup {
		std::map<DeviceContext*, vk::DescriptorSet> sets;
		~DescriptorSetGroup();
	};
	struct PipelineLayoutGroup {
		std::map<DeviceContext*, vk::PipelineLayout> sets;
		~PipelineLayoutGroup();
	};

	struct ShaderGroup {
		std::map<DeviceContext*, Shader*> sets;
		~ShaderGroup();
	};

	struct PipelineCacheGroup {
		std::map<DeviceContext*, vk::PipelineCache> sets;
		~PipelineCacheGroup();
	};

}