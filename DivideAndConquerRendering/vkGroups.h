#pragma once
#include <map>
#include <vulkan\vulkan.hpp>


class Shader;
class DeviceContext;
class Model;
namespace vkGroups {

	struct ShaderGroup {
		std::map<DeviceContext*, Shader*> sets;
		~ShaderGroup();
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

	struct ModelGroup
	{
		std::map<DeviceContext*, Model> sets;
		~ModelGroup();
	};
}