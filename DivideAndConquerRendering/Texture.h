#pragma once
#include <vulkan\vulkan.hpp>
#include "DeviceContext.h"

class Texture
{
protected:
	vk::Image image;
	vk::ImageView imageView;
	vk::Extent2D extends;
	vk::DeviceMemory imageMemory;
	DeviceContext* deviceContext;
	vk::Format format;
	void * memoryTest;
public:
	Texture(DeviceContext* deviceContext, uint32_t width, uint32_t height, vk::Format format, vk::ImageLayout layout, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags memoryProperties, vk::ImageAspectFlags aspectFlag);

	Texture(DeviceContext* deviceContext, uint32_t width, uint32_t height, vk::Format format, vk::ImageLayout layout, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags memoryProperties);

	~Texture();

	vk::ImageView& getImageView();
	vk::Image& getImage();
	vk::Extent2D& getExtends();
	vk::Format& getFormat();
	void transferTextureTo(Texture& destination);

	static Texture* loadFromFile(DeviceContext* deviceContext, std::string filename);
protected:
	void createImage(DeviceContext * deviceContext, vk::Format format, vk::ImageLayout layout, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags memoryProperties);
	void createImageView(DeviceContext * deviceContext, vk::Format format, vk::ImageAspectFlags aspectFlag);
	
};