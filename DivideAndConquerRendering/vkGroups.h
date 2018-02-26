#pragma once
#include <map>
#include <vulkan\vulkan.hpp>
#include "VertexBuffer.h"

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

	struct PipelineShaderStageGroup {
		std::map<DeviceContext*, vk::PipelineShaderStageCreateInfo> sets;
		~PipelineShaderStageGroup();
	};

	struct GraphicsPipelineCreateInfoGroup {
		std::map<DeviceContext*, vk::GraphicsPipelineCreateInfo> sets;
		~GraphicsPipelineCreateInfoGroup();
	};

	struct PipelineCacheGroup {
		std::map<DeviceContext*, vk::PipelineCache> sets;
		~PipelineCacheGroup();
	};

	struct VertexBufferGroup {
		std::map<DeviceContext*, VertexBuffer> sets;
		~VertexBufferGroup();
	};

}