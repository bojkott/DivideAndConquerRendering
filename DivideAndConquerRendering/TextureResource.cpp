#include "TextureResource.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Renderer.h"



TextureResource::TextureResource(DeviceContext * deviceContext, uint32_t width, uint32_t height, vk::Format format) : Texture(deviceContext, width, height, format,
	vk::ImageLayout::eUndefined,
	vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal)
{
}

int TextureResource::loadFromFile(std::string filename)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	vk::DeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels)
		throw std::runtime_error("Failed to load image!");

	//Fix so that we use vk instead of vkGroups
	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;

	//createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBufferGroup, stagingBufferMemoryGroup);
	//Renderer::deviceGroup.copyDataToGPUs(pixels, stagingBufferMemoryGroup, imageSize, 0, vk::MemoryMapFlagBits());	//Don't know what the last parameter is supposed to be. It's 0 in the c version but can't pass 0 here. So Will just pass the empty enum.
	//stbi_image_free(pixels);

	

	return 0;
}

void TextureResource::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vkGroups::BufferGroup& bufferGroup, vkGroups::BufferMemoryGroup& bufferMemoryGroup)
{
	vk::BufferCreateInfo bufferInfo;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;
	bufferGroup = Renderer::deviceGroup.createBuffer(bufferInfo);
	bufferMemoryGroup = Renderer::deviceGroup.allocateMemory(bufferGroup, properties);
	Renderer::deviceGroup.bindBufferMemory(bufferGroup, bufferMemoryGroup, 0);
}

void TextureResource::transitionImageLayout(vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	deviceContext->executeSingleTimeQueue(
		[this](vk::CommandBuffer commandBuffer)
	{
		VulkanHelpers::cmdTransitionLayout(
			commandBuffer,
			this->getImage(),
			vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
			{}, vk::AccessFlagBits::eMemoryWrite,
			vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer);
		/*VulkanHelpers::cmdCopyBufferToImage(
			commandBuffer,
			buffer,
		)*/
	});
}
