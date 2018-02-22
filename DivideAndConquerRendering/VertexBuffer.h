#pragma once
#include <vulkan/vulkan.hpp>
#include <SDL_vulkan.h>
#include "DeviceContext.h"


class VertexBuffer
{
public:
	VertexBuffer(std::vector<uint32_t> verts, DeviceContext& device);
};