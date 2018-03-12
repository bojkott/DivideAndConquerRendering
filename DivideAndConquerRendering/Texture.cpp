#include "Texture.h"
#include <fstream>
#include "Buffer.h"
#include "VulkanHelpers.h"
#include <thread>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(DeviceContext * deviceContext, uint32_t width, uint32_t height, vk::Format format, vk::ImageLayout layout, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags memoryProperties, vk::ImageAspectFlags aspectFlag)
{
	this->deviceContext = deviceContext;
	extends = { width, height };
	this->format = format;

	vk::ImageUsageFlags shouldCreateImageView = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eInputAttachment;

	createImage(deviceContext, format, layout, tiling, usage, memoryProperties);

	if(usage & shouldCreateImageView)
		createImageView(deviceContext, format, aspectFlag);
}

Texture::Texture(DeviceContext * deviceContext, uint32_t width, uint32_t height, vk::Format format, vk::ImageLayout layout, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags memoryProperties) :
	Texture(deviceContext, width, height, format, layout, tiling, usage, memoryProperties, vk::ImageAspectFlagBits::eColor)
{
}

Texture::~Texture()
{
	if(imageView)
		deviceContext->getDevice().destroyImageView(imageView);

	deviceContext->getDevice().freeMemory(imageMemory);
	deviceContext->getDevice().destroyImage(image);
}

vk::ImageView & Texture::getImageView()
{
	return imageView;
}

vk::Image & Texture::getImage()
{
	return image;
}

vk::Extent2D & Texture::getExtends()
{
	return extends;
}

void Texture::transferTextureTo(Texture & destination)
{

	size_t size = extends.width * extends.height * 4;

	float* dataA = (float*)this->deviceContext->getDevice().mapMemory(this->imageMemory, 0, size);

	float* dataB = (float*)destination.deviceContext->getDevice().mapMemory(destination.imageMemory, 0, size);

	memcpy(dataB, dataA, size);

	this->deviceContext->getDevice().unmapMemory(this->imageMemory);
	destination.deviceContext->getDevice().unmapMemory(destination.imageMemory);
}

Texture* Texture::loadFromFile(DeviceContext * deviceContext, std::string filename)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(("../assets/" + filename).c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	vk::DeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels)
		throw std::runtime_error("Failed to load image: " + filename);

	Texture* texture = new Texture(
		deviceContext, texWidth, texHeight, vk::Format::eB8G8R8A8Unorm,
		vk::ImageLayout::eUndefined,
		vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal);

	Buffer stagingBuffer = Buffer(
		deviceContext,
		imageSize,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::SharingMode::eExclusive,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
	);

	stagingBuffer.setData(pixels);

	stbi_image_free(pixels);

	deviceContext->executeSingleTimeQueue(
		[texture, &stagingBuffer](vk::CommandBuffer commandBuffer)
	{
		VulkanHelpers::cmdTransitionLayout(
			commandBuffer,
			texture->getImage(),					//The image		
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
			stagingBuffer.getBuffer(),
			texture->getImage(),
			vk::ImageLayout::eTransferDstOptimal,
			texture->getExtends().width,
			texture->getExtends().height,
			vk::ImageAspectFlagBits::eColor);

		VulkanHelpers::cmdTransitionLayout(
			commandBuffer,
			texture->getImage(),						//The image		
			vk::ImageLayout::eTransferDstOptimal,		//Old image layout	(Hint: Look above)
			vk::ImageLayout::eShaderReadOnlyOptimal,	//New image layout	
			vk::AccessFlagBits::eMemoryWrite,			//Old access mask	
			vk::AccessFlagBits::eShaderRead,			//New access mask	
			vk::PipelineStageFlagBits::eTransfer,		//Old pipeline	
			vk::PipelineStageFlagBits::eFragmentShader,	//New pipeline
			vk::ImageAspectFlagBits::eColor
		);
	});

	return texture;
}



void Texture::createImage(DeviceContext * deviceContext, vk::Format format, vk::ImageLayout layout, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags memoryProperties)
{
	
	vk::ImageCreateInfo imageInfo;
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.extent.width = extends.width;
	imageInfo.extent.height = extends.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;

	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = layout;
	imageInfo.usage = usage;
	imageInfo.samples = vk::SampleCountFlagBits::e1;

	image = deviceContext->getDevice().createImage(imageInfo);

	vk::MemoryRequirements memRequirements = deviceContext->getDevice().getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocInfo;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = deviceContext->findMemoryType(memRequirements.memoryTypeBits, memoryProperties);

	imageMemory = deviceContext->getDevice().allocateMemory(allocInfo);

	deviceContext->getDevice().bindImageMemory(image, imageMemory, 0);
}

void Texture::createImageView(DeviceContext * deviceContext, vk::Format format, vk::ImageAspectFlags aspectFlag)
{
	vk::ImageViewCreateInfo viewInfo = {};
	viewInfo.image = image;
	viewInfo.viewType = vk::ImageViewType::e2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlag;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;


	imageView = deviceContext->getDevice().createImageView(viewInfo);
}

vk::Format& Texture::getFormat()
{
	return format;
}