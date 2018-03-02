#include "vkGroups.h"
#include "DeviceContext.h"
#include "Shader.h"
#include "Model.h"

namespace vkGroups {

	ShaderGroup::~ShaderGroup()
	{
		for (auto const& set : sets)
		{
			delete set.second;
		}
	}

	BufferGroup::~BufferGroup()
	{
		for (auto const& set : sets)
		{
			set.first->getDevice().destroyBuffer(set.second);
		}
	}
	BufferMemoryGroup::~BufferMemoryGroup()
	{
		for (auto const& set : sets)
		{
			set.first->getDevice().freeMemory(set.second);
		}
	}

	TechniqueGroup::~TechniqueGroup()
	{
		for (auto const& set : sets)
		{
			delete set.second;
		}
	}
	UniformBufferGroup::~UniformBufferGroup()
	{
		for (auto const& set : sets)
		{
			delete set.second;
		}
	}
	MeshGroup::~MeshGroup()
	{
		for (auto & set : sets)
		{
			for (auto const& mesh : set.second)
				delete mesh;
			set.second.clear();
		}
	}
}
