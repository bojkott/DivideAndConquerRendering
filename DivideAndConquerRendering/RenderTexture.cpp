#include "RenderTexture.h"

RenderTexture::RenderTexture(DeviceContext * deviceContext, uint32_t width, uint32_t height, vk::Format format) : Texture(deviceContext, width, height, format,
	vk::ImageLayout::eUndefined,
	vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eDeviceLocal)
{

}