#pragma once
#include <vulkan\vulkan.hpp>
#include "DeviceContext.h"
class RenderTexture {
private:
	vk::Image image;
	vk::ImageView imageView;
	vk::Extent2D extends;
	vk::DeviceMemory imageMemory;
	DeviceContext* deviceContext;
public:
	RenderTexture(DeviceContext* deviceContext, uint32_t width, uint32_t height, vk::Format format);

	vk::ImageView& getImageView();
	vk::Extent2D& getExtends();

	void transferTextureTo(RenderTexture& destination);
private:
	void createImage(DeviceContext * deviceContext, vk::Format format);
	void createImageView(DeviceContext * deviceContext, vk::Format format);
};