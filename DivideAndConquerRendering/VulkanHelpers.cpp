#include "VulkanHelpers.h"

void VulkanHelpers::cmdTransitionLayout(vk::CommandBuffer commandBuffer, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::AccessFlags oldAccessMask, vk::AccessFlags newAccessMask, vk::PipelineStageFlags oldStage, vk::PipelineStageFlags newStage, vk::ImageAspectFlags aspectMask)
{
	vk::ImageMemoryBarrier barrier;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = oldAccessMask;
	barrier.dstAccessMask = newAccessMask;

	barrier.subresourceRange.aspectMask = aspectMask;

	commandBuffer.pipelineBarrier(oldStage, newStage, {}, {}, {}, barrier);
}

void VulkanHelpers::cmdCopyImageSimple(vk::CommandBuffer commandBuffer, vk::Image srcImage, vk::ImageLayout srcImageLayout,
	vk::Image dstImage, vk::ImageLayout dstImageLayout, uint32_t width, uint32_t height)
{
	vk::ImageCopy imageCopyInfo;
	imageCopyInfo.extent.width = width;
	imageCopyInfo.extent.height = height;
	imageCopyInfo.extent.depth = 1;
	imageCopyInfo.srcOffset = { 0,0 };
	imageCopyInfo.dstOffset = { 0,0 };

	vk::ImageSubresourceLayers subResource;
	subResource.aspectMask = vk::ImageAspectFlagBits::eColor;
	subResource.mipLevel = 0;
	subResource.baseArrayLayer = 0;
	subResource.layerCount = 1;
	imageCopyInfo.srcSubresource = subResource;
	imageCopyInfo.dstSubresource = subResource;

	commandBuffer.copyImage(srcImage, srcImageLayout, dstImage, dstImageLayout, imageCopyInfo);
}

void VulkanHelpers::cmdBlitSimple(vk::CommandBuffer commandBuffer, vk::Image srcImage, vk::ImageLayout srcImageLayout,
	vk::Image dstImage, vk::ImageLayout dstImageLayout, uint32_t width, uint32_t height, vk::Filter filter)
{
	vk::Offset3D blitSize;
	blitSize.x = width;
	blitSize.y = height;
	blitSize.z = 1;
	vk::ImageBlit imageBlitRegion;
	imageBlitRegion.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageBlitRegion.srcSubresource.layerCount = 1;
	imageBlitRegion.srcOffsets[1] = blitSize;
	imageBlitRegion.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageBlitRegion.dstSubresource.layerCount = 1;
	imageBlitRegion.dstOffsets[1] = blitSize;


	commandBuffer.blitImage(srcImage, srcImageLayout, dstImage, dstImageLayout, imageBlitRegion, filter);
}


void VulkanHelpers::cmdCopyBufferToImage(vk::CommandBuffer commandBuffer, vk::Buffer srcBuffer, vk::Image dstImage, vk::ImageLayout dstImageLayout, uint32_t width, uint32_t height, vk::ImageAspectFlagBits aspectMask)
{
	vk::BufferImageCopy region;
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = aspectMask;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};
	
	commandBuffer.copyBufferToImage(srcBuffer, dstImage, vk::ImageLayout::eTransferDstOptimal, 1, &region);

}

void VulkanHelpers::cmdCopyImageToBuffer(vk::CommandBuffer commandBuffer, vk::Image srcImage, vk::Buffer dstBuffer, vk::ImageLayout srcImageLayout, uint32_t width, uint32_t height, vk::ImageAspectFlagBits aspectMask)
{
	vk::BufferImageCopy region;
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = aspectMask;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	commandBuffer.copyImageToBuffer(srcImage, srcImageLayout, dstBuffer, 1, &region);
}

void VulkanHelpers::cmdCopyBuffer(vk::CommandBuffer& commandBuffer, const vk::Buffer& srcBuffer, vk::Buffer& dstBuffer, vk::DeviceSize size)
{
	vk::BufferCopy copyRegion = {};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size; 
	commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);
}
