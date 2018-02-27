#include "vkGroups.h"
#include "DeviceContext.h"
#include "Shader.h"
namespace vkGroups {

	ShaderGroup::~ShaderGroup()
	{
		for (auto const& set : sets)
		{
			delete set.second;
		}
	}

	VertexBufferGroup::~VertexBufferGroup()
	{
		for (auto& set : sets)
		{
			set.second.Destroy();
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
}
