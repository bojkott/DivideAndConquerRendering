#pragma once
#include <map>
#include <vulkan\vulkan.hpp>


class Shader;
class DeviceContext;
class Technique;
class UniformBuffer;
class Mesh;
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


	struct TechniqueGroup
	{
		std::map < DeviceContext*, Technique*> sets;
		~TechniqueGroup();
	};

	struct UniformBufferGroup
	{
		std::map < DeviceContext*, UniformBuffer*> sets;
		~UniformBufferGroup();
	};

	struct MeshGroup
	{
		std::map < DeviceContext*, std::vector<Mesh*>> sets;
		~MeshGroup();
	};
}