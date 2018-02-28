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

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	DeviceContext& tempContext = *deviceContext;
	deviceContext->executeSingleTimeQueue(
		[this, imageSize, &stagingBuffer, &stagingBufferMemory, &tempContext](vk::CommandBuffer commandBuffer)
	{
		VulkanHelpers::createBuffer(
			imageSize,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			stagingBuffer,
			stagingBufferMemory,
			tempContext
		);
	});

	void* data;
	copyDatatoGPU(stagingBufferMemory, pixels, 0, vk::MemoryMapFlagBits());
	stbi_image_free(pixels);

	createImage(deviceContext, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal);
	deviceContext->executeSingleTimeQueue(
		[this, stagingBuffer, texWidth, texHeight](vk::CommandBuffer commandBuffer)
	{
		VulkanHelpers::cmdTransitionLayout(
			commandBuffer,
			this->getImage(),						//The image		
			vk::ImageLayout::eUndefined,			//Old image layout
			vk::ImageLayout::eTransferDstOptimal,	//New image layout
			{},										//Old access mask
			vk::AccessFlagBits::eMemoryWrite,		//New access mask
			vk::PipelineStageFlagBits::eTopOfPipe,	//Old pipeline
			vk::PipelineStageFlagBits::eTransfer,	//New pipeline
			vk::ImageAspectFlagBits::eColor
		);
		VulkanHelpers::cmdCopyBufferToImage(
			commandBuffer,
			stagingBuffer,
			this->getImage(),
			vk::ImageLayout::eTransferDstOptimal,
			texWidth,
			texHeight,
			vk::ImageAspectFlagBits::eColor
		);
		VulkanHelpers::cmdTransitionLayout(
			commandBuffer,									
			this->getImage(),							//The image		
			vk::ImageLayout::eTransferDstOptimal,		//Old image layout	(Hint: Look above)
			vk::ImageLayout::eShaderReadOnlyOptimal,	//New image layout	
			vk::AccessFlagBits::eMemoryWrite,			//Old access mask	
			vk::AccessFlagBits::eShaderRead,			//New access mask	
			vk::PipelineStageFlagBits::eTransfer,		//Old pipeline	
			vk::PipelineStageFlagBits::eFragmentShader,	//New pipeline
			vk::ImageAspectFlagBits::eColor
		);
	});

	deviceContext->getDevice().destroyBuffer(stagingBuffer);
	deviceContext->getDevice().freeMemory(stagingBufferMemory);

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
	
}
