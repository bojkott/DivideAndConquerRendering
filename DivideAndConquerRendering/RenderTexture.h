#pragma once
#include <vulkan\vulkan.hpp>
#include "DeviceContext.h"
class RenderTexture {
private:
	vk::Image image;
	vk::ImageView imageView;
	vk::Extent2D extends;
	vk::DeviceMemory imageMemory;
public:
	RenderTexture(DeviceContext* deviceContext, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
		vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);

	vk::ImageView& getImageView();
	vk::Extent2D& getExtends();
private:
	void createImage(DeviceContext * deviceContext, vk::Format format, vk::ImageTiling tiling,
		vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
	void createImageView(DeviceContext * deviceContext, vk::Format format);
};