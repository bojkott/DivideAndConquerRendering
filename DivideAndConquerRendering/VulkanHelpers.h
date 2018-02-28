#pragma once
#include "DeviceContext.h"
#include <vulkan\vulkan.hpp>
class VulkanHelpers
{
public:
	static void cmdTransitionLayout(vk::CommandBuffer commandBuffer, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
		vk::AccessFlags oldAccessMask, vk::AccessFlags newAccessMask, vk::PipelineStageFlags oldStage, vk::PipelineStageFlags newStage, vk::ImageAspectFlagBits aspectMask);

	//Copy image with same extends and no offset.
	static void cmdCopyImageSimple(vk::CommandBuffer commandBuffer, vk::Image srcImage, vk::ImageLayout srcImageLayout,
		vk::Image dstImage, vk::ImageLayout dstImageLayout, uint32_t width, uint32_t height);

	static void cmdBlitSimple(vk::CommandBuffer commandBuffer, vk::Image srcImage, vk::ImageLayout srcImageLayout,
		vk::Image dstImage, vk::ImageLayout dstImageLayout, uint32_t width, uint32_t height, vk::Filter filter);

	static void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties,
		vk::Buffer& buffer, vk::DeviceMemory& bufferMemory, DeviceContext& const context);

	static void cmdCopyBufferToImage(vk::CommandBuffer commandBuffer, vk::Buffer srcBuffer, vk::Image dstImage, vk::ImageLayout dstImageLayout, uint32_t width, uint32_t height, vk::ImageAspectFlagBits aspectMask);
	static void cmdCopyImageToBuffer(vk::CommandBuffer commandBuffer, vk::Image srcImage, vk::Buffer dstBuffer, vk::ImageLayout srcImageLayout, uint32_t width, uint32_t height, vk::ImageAspectFlagBits aspectMask);

	static void cmdCopyBuffer(vk::CommandBuffer& commandBuffer, const vk::Buffer& srcBuffer, vk::Buffer& dstBuffer, vk::DeviceSize size);

};