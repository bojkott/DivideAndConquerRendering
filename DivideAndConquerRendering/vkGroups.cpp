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

	ModelGroup::~ModelGroup()
	{
		for (auto const& set : sets)
		{
			delete set.second;
		}
	}
	TechniqueGroup::~TechniqueGroup()
	{
		for (auto const& set : sets)
		{
			delete set.second;
		}
	}
}
