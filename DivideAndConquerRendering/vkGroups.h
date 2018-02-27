#pragma once
#include <map>
#include <vulkan\vulkan.hpp>
#include "VertexBuffer.h"

class Shader;
class DeviceContext;
namespace vkGroups {

	struct ShaderGroup {
		std::map<DeviceContext*, Shader*> sets;
		~ShaderGroup();
	};

	struct VertexBufferGroup {
		std::map<DeviceContext*, VertexBuffer> sets;
		~VertexBufferGroup();
	};

	struct BufferGroup
	{
		std::map<DeviceContext*, vk::Buffer> sets;
		~BufferGroup();
	};

	struct BufferMemoryGroup
	{
		std::map<DeviceContext*, vk::DeviceMemory> sets;
		~BufferMemoryGroup();
	};
}