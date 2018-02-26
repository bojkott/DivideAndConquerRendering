#include "DeviceContext.h"
#include <set>
#include "Renderer.h"
#include "Window.h"
#include "DeviceGroup.h"
#include "RenderTexture.h"
#include "VulkanHelpers.h"
DeviceContext::DeviceContext(DeviceGroup* group, vk::Instance & instance, vk::PhysicalDevice physicalDevice): mode(DEVICE_MODE::HEADLESS), deviceGroup(group), physicalDevice(physicalDevice)
{
	createDevice(instance);
}

DeviceContext::DeviceContext(DeviceGroup* group, vk::Instance & instance, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR& surface) : mode(DEVICE_MODE::WINDOW), deviceGroup(group), physicalDevice(physicalDevice), surface(surface)
{
	createDevice(instance);
}

void DeviceContext::initDevice()
{
	if (mode == DEVICE_MODE::HEADLESS) 
	{
		createRenderPass();
		createRenderTexture();
		createCommandPool();
		createCommandBuffers();
		createSemaphores();
	}
	else
	{
		createSwapchain();
		createRenderPass();
		createPresentRenderPass();
		createFrameBuffers();
		createCommandPool();
		createCommandBuffers();
		createSemaphores();
		
		for (DeviceContext* deviceContext : deviceGroup->getDevices()) //one for each subdevice. so -1 to remove the primary device.
		{
			if (deviceContext == this)
				continue;
			Texture* targetTexture = new Texture(this,
				getMainDevice()->swapchain.extent.width,
				getMainDevice()->swapchain.extent.height,
				swapchain.imageFormat,
				vk::ImageLayout::eUndefined,
				vk::ImageTiling::eLinear,
				vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

			executeSingleTimeQueue(
				[targetTexture](vk::CommandBuffer commandBuffer)
			{
				VulkanHelpers::cmdTransitionLayout(
					commandBuffer,
					targetTexture->getImage(),
					vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral,
					{}, vk::AccessFlagBits::eMemoryWrite,
					vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer);
			});

			targetTextures.insert(std::make_pair(deviceContext, targetTexture));
		}

		

	}
}

DeviceContext::~DeviceContext()
{

	for (auto texMap : targetTextures)
		delete texMap.second;

	targetTextures.clear();

	device.destroyCommandPool(commandPool);
	for (auto framebuffer : swapchain.framebuffers) {
		device.destroyFramebuffer(framebuffer);
	}

	device.destroyRenderPass(renderPass);

	if (mode == DEVICE_MODE::WINDOW) {

		device.destroyRenderPass(presentRenderPass);

		for (vk::ImageView& view : swapchain.imageViews) {
			device.destroyImageView(view);
		}
		device.destroySwapchainKHR(swapchain.swapchain);
	}

	device.destroySemaphore(imageAvailableSemaphore);
	device.destroySemaphore(renderFinishedSemaphore);

	device.destroy();
	
}

vk::Device & DeviceContext::getDevice()
{
	return device;
}

vk::Device * DeviceContext::getAddressOfDevice()
{
	return &device;
}

vk::RenderPass & DeviceContext::getRenderpass()
{
	return renderPass;
}

std::map<DeviceContext*, Texture*> DeviceContext::getTargetTextures()
{
	return targetTextures;
}

vk::CommandPool DeviceContext::getCommandPool()
{
	return commandPool;
}

void DeviceContext::clearBuffer(float r, float g, float b, float a)
{
	vk::ClearValue clearColor(std::array<float, 4>{r, g, b, a});
	device.resetCommandPool(commandPool, vk::CommandPoolResetFlagBits::eReleaseResources);

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
	

	vk::RenderPassBeginInfo renderPassInfo;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.renderArea.offset = { 0, 0 };
	
	renderPassInfo.clearValueCount = 1;


	renderPassInfo.pClearValues = &clearColor;

	if (mode == DEVICE_MODE::HEADLESS)
	{
		renderPassCommandBuffer.begin(beginInfo);
		renderPassInfo.renderArea.extent = renderTexture->getExtends();
		renderPassInfo.framebuffer = renderTextureFrameBuffer;

		renderPassCommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	
	}

	for (size_t i = 0; i < swapchain.commandBuffers.size(); i++)
	{
		vk::CommandBuffer& commandBuffer = swapchain.commandBuffers[i];

		commandBuffer.begin(beginInfo);

		renderPassInfo.framebuffer = swapchain.framebuffers[i];
		renderPassInfo.renderArea.extent = swapchain.extent;

		
		commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
		commandBuffer.endRenderPass();

	}
	
}

void DeviceContext::tempPresent()
{

	uint32_t imageIndex = device.acquireNextImageKHR(swapchain.swapchain, UINT64_MAX, imageAvailableSemaphore, vk::Fence()).value;


	vk::SubmitInfo submitInfo;

	vk::Semaphore waitSemaphores[] = { imageAvailableSemaphore };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &swapchain.commandBuffers[imageIndex];

	vk::Semaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	graphicsQueue.submit(1, &submitInfo, vk::Fence());

	vk::PresentInfoKHR presentInfo = {};

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	vk::SwapchainKHR swapChains[] = { swapchain.swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	presentQueue.presentKHR(presentInfo);
	presentQueue.waitIdle();
}

void DeviceContext::executeCommandQueue()
{

	vk::SubmitInfo submitInfo;


	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &renderPassCommandBuffer;

	vk::Semaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	graphicsQueue.submit(1, &submitInfo, vk::Fence());

	submitInfo.commandBufferCount = 0;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = signalSemaphores;
	submitInfo.signalSemaphoreCount = 0;

	graphicsQueue.submit(1, &submitInfo, vk::Fence());

	graphicsQueue.waitIdle();

}

void DeviceContext::executeSingleTimeQueue(std::function< void(vk::CommandBuffer)> commands)
{
	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;
	
	vk::CommandBuffer commandBuffer = device.allocateCommandBuffers(allocInfo)[0];

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	commandBuffer.begin(beginInfo);

	commands(commandBuffer);

	commandBuffer.end();
	
	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	graphicsQueue.submit(submitInfo, {});
	graphicsQueue.waitIdle();
	device.freeCommandBuffers(commandPool, commandBuffer);
}

void DeviceContext::transferRenderTexture()
{
	if (mode == DEVICE_MODE::HEADLESS)
	{
	
		//this should not be here later :)
		renderPassCommandBuffer.endRenderPass();

		Texture* targetTexture = targetTextures[this];

		VulkanHelpers::cmdTransitionLayout(
			renderPassCommandBuffer,
			targetTexture->getImage(),
			vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
			{}, vk::AccessFlagBits::eTransferWrite,
			vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer);


		VulkanHelpers::cmdCopyImageSimple(renderPassCommandBuffer,
			renderTexture->getImage(), vk::ImageLayout::eTransferSrcOptimal,
			targetTexture->getImage(), vk::ImageLayout::eTransferDstOptimal,
			renderTexture->getExtends().width, renderTexture->getExtends().height);


		VulkanHelpers::cmdTransitionLayout(
			renderPassCommandBuffer,
			targetTexture->getImage(),
			vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eGeneral,
			vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eTransferRead,
			vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer);

		renderPassCommandBuffer.end();
	}
}

uint32_t DeviceContext::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("Failed to find memory type");
}

void DeviceContext::createDevice(vk::Instance & instance)
{
	QueueFamilyIndices indices = findQueueFamilies();
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = { indices.graphicsFamily};
	if (mode == DEVICE_MODE::WINDOW)
		uniqueQueueFamilies.insert(indices.presentFamily);

	float queuePriority = 1.0f;
	for (int queueFamily : uniqueQueueFamilies)
	{
		vk::DeviceQueueCreateInfo queueCreateInfo;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	vk::PhysicalDeviceFeatures deviceFeatures; //Enable features here? Or enable all available?

	std::vector<const char*> deviceExtensions;
	if (mode == DEVICE_MODE::WINDOW)
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	vk::DeviceCreateInfo createInfo;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (Renderer::enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(Renderer::validationLayers.size());
		createInfo.ppEnabledLayerNames = Renderer::validationLayers.data();
	}
	else
		createInfo.enabledLayerCount = 0;


	device = physicalDevice.createDevice(createInfo);

	graphicsQueue = device.getQueue(indices.graphicsFamily, 0);
	if(mode == DEVICE_MODE::WINDOW)
		presentQueue = device.getQueue(indices.presentFamily, 0);

}

void DeviceContext::createRenderPass()
{
	//Create standard render pass that will write to an image that will be used in the next pass.
	vk::AttachmentDescription colorAttachment;
	colorAttachment.format = getMainDevice()->swapchain.imageFormat; //maybe hardcode?
	colorAttachment.samples = vk::SampleCountFlagBits::e1;
	colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eClear;
	colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eStore;
	colorAttachment.initialLayout = vk::ImageLayout::eUndefined;

	//If we are the main GPU we will set our final layout to an image to be used for input in the next pass.
	//If we are a slave GPU we will set our layout to an image to be used as a source for a copy operation.
	switch (mode)
	{
		case DEVICE_MODE::WINDOW:
			colorAttachment.finalLayout = vk::ImageLayout::eTransferDstOptimal;
			break;
		case DEVICE_MODE::HEADLESS:
			colorAttachment.finalLayout = vk::ImageLayout::eTransferSrcOptimal;
			break;
	}
	

	vk::AttachmentReference colorAttachmentRef;
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;


	vk::SubpassDescription subpass;
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;


	vk::RenderPassCreateInfo renderPassInfo;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	renderPass = device.createRenderPass(renderPassInfo);

}

void DeviceContext::createPresentRenderPass()
{
	//Create final render pass that will write to an image that will be used in the next pass.
	vk::AttachmentDescription colorAttachment;
	colorAttachment.format = swapchain.imageFormat; //maybe hardcode?
	colorAttachment.samples = vk::SampleCountFlagBits::e1;
	colorAttachment.loadOp = vk::AttachmentLoadOp::eLoad;
	colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eLoad;
	colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	colorAttachment.initialLayout = vk::ImageLayout::eTransferDstOptimal;
	colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

	vk::AttachmentReference colorAttachmentRef;
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

	vk::SubpassDescription subpass;
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;


	vk::RenderPassCreateInfo renderPassInfo;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	presentRenderPass = device.createRenderPass(renderPassInfo);
}

void DeviceContext::createFrameBuffers()
{
	int frameBufferCount = swapchain.imageViews.size();
	swapchain.framebuffers.resize(frameBufferCount);

	//create an image view for the output to the swap texture.
	for (size_t i = 0; i < frameBufferCount; i++) {
		vk::ImageView attachments[] = {swapchain.imageViews[i]};

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = presentRenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapchain.extent.width;
		framebufferInfo.height = swapchain.extent.height;
		framebufferInfo.layers = 1;

		swapchain.framebuffers[i] = device.createFramebuffer(framebufferInfo);
	}

}

void DeviceContext::createRenderTexture()
{

	renderTexture = new RenderTexture(this,
		getMainDevice()->swapchain.extent.width,
		getMainDevice()->swapchain.extent.height,
		getMainDevice()->swapchain.imageFormat);

	targetTextures.insert(
		std::make_pair(this,
			new Texture(this,
		getMainDevice()->swapchain.extent.width,
		getMainDevice()->swapchain.extent.height,
		getMainDevice()->swapchain.imageFormat,
		vk::ImageLayout::eUndefined,
		vk::ImageTiling::eLinear,
		vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)));

	vk::ImageView attachments[] = { renderTexture->getImageView() };

	vk::FramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = attachments;
	framebufferInfo.width = renderTexture->getExtends().width;
	framebufferInfo.height = renderTexture->getExtends().height;
	framebufferInfo.layers = 1;



	renderTextureFrameBuffer = device.createFramebuffer(framebufferInfo);
}

void DeviceContext::createSwapchain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport();

	vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	// One more backbuffer frame than minimal, let's play it safe yo.
	// Tripplebuffering.
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 &&
		imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	vk::SwapchainCreateInfoKHR createInfo;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1; // 1 if not VR
	createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;


	QueueFamilyIndices indices = findQueueFamilies();
	std::vector<uint32_t> queueFamilyIndices = { (uint32_t)indices.graphicsFamily };
	if (mode == DEVICE_MODE::WINDOW)
		queueFamilyIndices.push_back((uint32_t)indices.presentFamily);

	if (indices.graphicsFamily != indices.presentFamily && mode == DEVICE_MODE::WINDOW)
	{
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
		createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
	}
	else
	{
		createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	swapchain.swapchain = device.createSwapchainKHR(createInfo);

	swapchain.images = device.getSwapchainImagesKHR(swapchain.swapchain);
	swapchain.imageFormat = surfaceFormat.format;
	swapchain.extent = extent;

	createSwapchainImageViews();
}

void DeviceContext::createSwapchainImageViews()
{
	swapchain.imageViews.resize(swapchain.images.size());
	for (size_t i = 0; i < swapchain.images.size(); ++i)
	{
		vk::ImageViewCreateInfo createInfo;
		createInfo.image = swapchain.images[i];
		createInfo.viewType = vk::ImageViewType::e2D;
		createInfo.format = swapchain.imageFormat;
		createInfo.components.r = vk::ComponentSwizzle::eIdentity;
		createInfo.components.g = vk::ComponentSwizzle::eIdentity;
		createInfo.components.b = vk::ComponentSwizzle::eIdentity;
		createInfo.components.a = vk::ComponentSwizzle::eIdentity;
		createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		swapchain.imageViews[i] = device.createImageView(createInfo);
	}
}

void DeviceContext::createCommandBuffers()
{
	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.commandPool = commandPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	if (mode == DEVICE_MODE::HEADLESS)
	{
		allocInfo.commandBufferCount = 1;
		renderPassCommandBuffer = device.allocateCommandBuffers(allocInfo)[0];
	}
	else
	{
		swapchain.commandBuffers.resize(swapchain.framebuffers.size());
		allocInfo.commandBufferCount = (uint32_t)swapchain.commandBuffers.size();

		swapchain.commandBuffers = device.allocateCommandBuffers(allocInfo);
	}
	
}

void DeviceContext::createSemaphores()
{
	//TODO more things
	vk::SemaphoreCreateInfo semaphoreInfo;

	imageAvailableSemaphore = device.createSemaphore(semaphoreInfo);
	renderFinishedSemaphore = device.createSemaphore(semaphoreInfo);
}

void DeviceContext::startFinalRenderPass(vk::Pipeline combineTechnique)
{
	vk::RenderPassBeginInfo finalPassInfo;
	finalPassInfo.renderPass = presentRenderPass;
	finalPassInfo.renderArea.offset = { 0, 0 };

	finalPassInfo.clearValueCount = 0;

	for (size_t i = 0; i < swapchain.commandBuffers.size(); i++)
	{
		vk::CommandBuffer& commandBuffer = swapchain.commandBuffers[i];


		finalPassInfo.framebuffer = swapchain.framebuffers[i];
		finalPassInfo.renderArea.extent = swapchain.extent;

		

		for (auto& texMap : targetTextures)
		{
			Texture* targetTexture = texMap.second;

			VulkanHelpers::cmdTransitionLayout(
				commandBuffer,
				targetTexture->getImage(),
				vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal,
				vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eMemoryRead,
				vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer);

			VulkanHelpers::cmdBlitSimple(
				commandBuffer,
				targetTexture->getImage(), vk::ImageLayout::eTransferSrcOptimal,
				swapchain.images[i], vk::ImageLayout::eTransferDstOptimal,
				swapchain.extent.width, swapchain.extent.height,
				vk::Filter::eNearest);

			VulkanHelpers::cmdTransitionLayout(
				commandBuffer,
				targetTexture->getImage(),
				vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eGeneral,
				vk::AccessFlagBits::eMemoryRead, vk::AccessFlagBits::eMemoryWrite,
				vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer);
		}

		commandBuffer.beginRenderPass(finalPassInfo, vk::SubpassContents::eInline);

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, combineTechnique);

		commandBuffer.draw(3, 1, 0, 0);

		commandBuffer.endRenderPass();
		commandBuffer.end();
	}
}

void DeviceContext::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies();

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
	poolInfo.flags = 0; // Optional

	commandPool = device.createCommandPool(poolInfo);
}

DeviceContext::QueueFamilyIndices DeviceContext::findQueueFamilies()
{
	QueueFamilyIndices indices;
	std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

	int i = 0;
	VkBool32 presentSupport = false;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
			indices.graphicsFamily = i;

		if (mode == DEVICE_MODE::WINDOW)
		{
			presentSupport = physicalDevice.getSurfaceSupportKHR(i, surface);
			if (queueFamily.queueCount > 0 && presentSupport)
				indices.presentFamily = i;
		}
		else
			indices.headless = true;

		if (indices.isComplete())
			break;

		i++;
	}
	return indices;
}

DeviceContext::SwapChainSupportDetails DeviceContext::querySwapChainSupport()
{

	SwapChainSupportDetails details;
	details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	details.formats = physicalDevice.getSurfaceFormatsKHR(surface);
	details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
	return details;
}

vk::SurfaceFormatKHR DeviceContext::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	//changed from b8g8r8a8 to r8g8b8a8
	if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
	{
		return { vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
	}

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == vk::Format::eR8G8B8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return availableFormat;
		}
	}
	if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
	{
		return { vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
	}

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == vk::Format::eR8G8B8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

vk::PresentModeKHR DeviceContext::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes)
{
	vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == vk::PresentModeKHR::eFifo)
		{
			return availablePresentMode;
		}
		else if (availablePresentMode == vk::PresentModeKHR::eImmediate)
		{
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

vk::Extent2D DeviceContext::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR & capabilities)
{

	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		vk::Extent2D actualExtent = { Window::getWidth(), Window::getHeight() };

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

DeviceContext * DeviceContext::getMainDevice()
{
	switch (mode)
	{
	case DEVICE_MODE::WINDOW:
		return this;
		break;
	case DEVICE_MODE::HEADLESS:
		return deviceGroup->getMainDevice();
		break;
	}
}

DeviceContext::Swapchain & DeviceContext::getSwapchain()
{
	return swapchain;
}
