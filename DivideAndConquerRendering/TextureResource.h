#pragma once
#include "Texture.h"
#include "DeviceGroup.h"
#include <vulkan\vulkan.hpp>

class TextureResource : public Texture
{
private:

public:
	TextureResource(DeviceContext* deviceContext, uint32_t width, uint32_t height, vk::Format format);
	int loadFromFile(std::string filename);
private:
	void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vkGroups::BufferGroup& bufferGroup, vkGroups::BufferMemoryGroup& bufferMemoryGroup);
	void transitionImageLayout(vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
};