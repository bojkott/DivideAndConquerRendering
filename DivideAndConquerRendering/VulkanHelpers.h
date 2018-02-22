#pragma once
#include <vulkan\vulkan.hpp>
class VulkanHelpers
{
public:
	static void cmdTransitionLayout(vk::CommandBuffer commandBuffer, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
		vk::AccessFlags oldAccessMask, vk::AccessFlags newAccessMask, vk::PipelineStageFlags oldStage, vk::PipelineStageFlags newStage);

	//Copy image with same extends and no offset.
	static void cmdCopyImageSimple(vk::CommandBuffer commandBuffer, vk::Image srcImage, vk::ImageLayout srcImageLayout,
		vk::Image dstImage, vk::ImageLayout dstImageLayout, uint32_t width, uint32_t height);

};