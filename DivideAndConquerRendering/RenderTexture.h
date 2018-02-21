#pragma once
#include <vulkan\vulkan.hpp>
#include "DeviceContext.h"
#include "Texture.h"
class RenderTexture : public Texture {
public:
	RenderTexture(DeviceContext* deviceContext, uint32_t width, uint32_t height, vk::Format format);
};