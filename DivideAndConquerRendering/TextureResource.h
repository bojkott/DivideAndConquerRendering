#pragma once
#include "Texture.h"
#include <vulkan\vulkan.hpp>

class TextureResource : public Texture
{
private:

public:
	int loadFromFile(std::string filename);
private:
	void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);

};