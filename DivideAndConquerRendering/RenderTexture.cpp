#include "RenderTexture.h"

RenderTexture::RenderTexture(DeviceContext * deviceContext, uint32_t width, uint32_t height, vk::Format format)
{
	this->deviceContext = deviceContext;
	extends = { width, height };
	createImage(deviceContext, format);
	createImageView(deviceContext, format);
}

vk::ImageView & RenderTexture::getImageView()
{
	return imageView;
}

vk::Extent2D & RenderTexture::getExtends()
{
	return extends;
}

void RenderTexture::transferTextureTo(RenderTexture & destination)
{
	/*size_t size = extends.width * extends.height * 4;
	void* dataA = this->deviceContext->getDevice().mapMemory(this->imageMemory, 0, size);

	void* dataB = destination.deviceContext->getDevice().mapMemory(destination.imageMemory, 0, size);

	memcpy(dataB, dataA, size);
	this->deviceContext->getDevice().unmapMemory(this->imageMemory);
	destination.deviceContext->getDevice().unmapMemory(destination.imageMemory);
*/

}

void RenderTexture::createImage(DeviceContext * deviceContext, vk::Format format)
{
	
	vk::ImageCreateInfo imageInfo;
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.extent.width = extends.width;
	imageInfo.extent.height = extends.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;

	imageInfo.format = format;
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.initialLayout = vk::ImageLayout::eUndefined;
	imageInfo.usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc;
	imageInfo.samples = vk::SampleCountFlagBits::e1;

	image = deviceContext->getDevice().createImage(imageInfo);

	vk::MemoryRequirements memRequirements = deviceContext->getDevice().getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocInfo;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = deviceContext->findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

	imageMemory = deviceContext->getDevice().allocateMemory(allocInfo);

	deviceContext->getDevice().bindImageMemory(image, imageMemory, 0);

}

void RenderTexture::createImageView(DeviceContext* deviceContext, vk::Format format)
{
	vk::ImageViewCreateInfo viewInfo = {};
	viewInfo.image = image;
	viewInfo.viewType = vk::ImageViewType::e2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;


	imageView = deviceContext->getDevice().createImageView(viewInfo);
}
