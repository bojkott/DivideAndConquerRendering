#include "TextureResource.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int TextureResource::loadFromFile(std::string filename)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	vk::DeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels)
		throw std::runtime_error("Failed to load image!");

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;

	void* data;

	

	return 0;
}

void TextureResource::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer & buffer, vk::DeviceMemory & bufferMemory)
{
	vk::BufferCreateInfo bufferInfo;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	//Make a vkGroup for vk::Buffer


	
}
