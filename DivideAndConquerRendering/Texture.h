#pragma once
#include <vulkan\vulkan.hpp>
#include "DeviceContext.h"
#include "stb_image.h"

class Texture
{
protected:
	vk::Image image;
	vk::ImageView imageView;
	vk::Extent2D extends;
	vk::DeviceMemory imageMemory;
	DeviceContext* deviceContext;
public:
	Texture(DeviceContext* deviceContext, uint32_t width, uint32_t height, vk::Format format, vk::ImageLayout layout, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags memoryProperties);
	void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
	vk::ImageView& getImageView();
	vk::Image& getImage();
	vk::Extent2D& getExtends();
	void transferTextureTo(Texture& destination);

private:
	void createImage(DeviceContext * deviceContext, vk::Format format, vk::ImageLayout layout, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags memoryProperties);
	void createImageView(DeviceContext * deviceContext, vk::Format format);
	vk::CommandBuffer beginSingleTimeCommands();
protected:
	void copyDatatoGPU(vk::DeviceMemory bufferMemory, stbi_uc* data, vk::DeviceSize offset, vk::MemoryMapFlags flasg);
};