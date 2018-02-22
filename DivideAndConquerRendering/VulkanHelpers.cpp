#include "VulkanHelpers.h"

void VulkanHelpers::cmdTransitionLayout(vk::CommandBuffer commandBuffer, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::AccessFlags oldAccessMask, vk::AccessFlags newAccessMask, vk::PipelineStageFlags oldStage, vk::PipelineStageFlags newStage)
{
	vk::ImageMemoryBarrier barrier;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = oldAccessMask;
	barrier.dstAccessMask = newAccessMask;

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
