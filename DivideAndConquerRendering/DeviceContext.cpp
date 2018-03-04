#include "DeviceContext.h"
#include <set>
#include "Renderer.h"
#include "Window.h"
#include "DeviceGroup.h"
#include "RenderTexture.h"
#include "VulkanHelpers.h"
#include "Technique.h"
#include "Buffer.h"
#include "Mesh.h"
#include "Camera.h"
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
		createCommandPool();
		createSecondaryDeviceTexture(this);
		createSecondaryDeviceFramebuffer();
		
		createCommandBuffers();
		createSemaphores();
	}
	else
	{
		createCommandPool();
		createSwapchain();
		createRenderPass();
		
				
		for (DeviceContext* deviceContext : deviceGroup->getDevices()) //one for each subdevice. so -1 to remove the primary device.
		{
			if (deviceContext == this)
				continue;

			createSecondaryDeviceTexture(deviceContext);
		}
		createPresentRenderPass();
		createFrameBuffers();
		
		createCommandBuffers();
		createSemaphores();

		

	}
}

void DeviceContext::setCombineTechnique(Technique * combineTechnique)
{
	this->combineTechnique = combineTechnique;

	std::vector<vk::WriteDescriptorSet> writes;
	

	vk::DescriptorImageInfo finalDepthImage;
	finalDepthImage.imageLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;
	finalDepthImage.imageView = swapchain.depthImage->getImageView();
	finalDepthImage.sampler = nullptr;

	vk::WriteDescriptorSet finalDepthWrite;

	finalDepthWrite.descriptorCount = 1;
	finalDepthWrite.dstSet = combineTechnique->getDescriptionSets()[0];
	finalDepthWrite.dstBinding = 0;
	finalDepthWrite.descriptorType = vk::DescriptorType::eInputAttachment;
	finalDepthWrite.pImageInfo = &finalDepthImage;
	finalDepthWrite.pBufferInfo = nullptr;
	finalDepthWrite.pTexelBufferView = nullptr;
	finalDepthWrite.dstArrayElement = 0;

	writes.push_back(finalDepthWrite);

	int i = 1;
	for (auto& texturePairs : secondaryDeviceTextures)
	{
		vk::DescriptorImageInfo renderInputImageInfo;
		renderInputImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		renderInputImageInfo.imageView = texturePairs.second->renderTexture->getImageView();
		renderInputImageInfo.sampler = nullptr;

		vk::DescriptorImageInfo depthInputImageInfo;
		depthInputImageInfo.imageLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;
		depthInputImageInfo.imageView = texturePairs.second->renderDepthTexture->getImageView();
		depthInputImageInfo.sampler = nullptr;

		vk::WriteDescriptorSet renderWrite;

		renderWrite.descriptorCount = 1;
		renderWrite.dstSet = combineTechnique->getDescriptionSets()[0];
		renderWrite.dstBinding = i;
		renderWrite.descriptorType = vk::DescriptorType::eInputAttachment;
		renderWrite.pImageInfo = &renderInputImageInfo;
		renderWrite.pBufferInfo = nullptr;
		renderWrite.pTexelBufferView = nullptr;
		renderWrite.dstArrayElement = 0;

		writes.push_back(renderWrite);
		
		vk::WriteDescriptorSet depthWrite;

		depthWrite.descriptorCount = 1;
		depthWrite.dstSet = combineTechnique->getDescriptionSets()[0];
		depthWrite.dstBinding = i+1;
		depthWrite.descriptorType = vk::DescriptorType::eInputAttachment;
		depthWrite.pImageInfo = &depthInputImageInfo;
		depthWrite.pBufferInfo = nullptr;
		depthWrite.pTexelBufferView = nullptr;
		depthWrite.dstArrayElement = 0;

		writes.push_back(depthWrite);

		i += 2;
	}

	device.updateDescriptorSets(writes, {});
}

DeviceContext::~DeviceContext()
{

	for (auto secondaryDevicePair : secondaryDeviceTextures)
		delete secondaryDevicePair.second;

	secondaryDeviceTextures.clear();

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

vk::RenderPass & DeviceContext::getRenderpass(RENDERPASS_TYPE type)
{
	switch (type)
	{
	case RENDERPASS_TYPE::Final:
		return presentRenderPass;
		break;
	case RENDERPASS_TYPE::Standard:
		return renderPass;
		break;
	}
	
}


vk::CommandPool DeviceContext::getCommandPool()
{
	return commandPool;
}

void DeviceContext::clearBuffer(float r, float g, float b, float a)
{
	std::vector<vk::ClearValue> clearValues(2);
	clearValues[0].color = (std::array<float, 4>{r, g, b, a});
	clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0.0f);
	

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
	

	vk::RenderPassBeginInfo renderPassInfo;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.renderArea.offset = { 0, 0 };
	
	renderPassInfo.clearValueCount = clearValues.size();

	renderPassInfo.pClearValues = clearValues.data();

	if (mode == DEVICE_MODE::HEADLESS)
	{
		renderPassCommandBuffer.begin(beginInfo);

		renderPassInfo.renderArea.extent = getMainDevice()->swapchain.extent;
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
	}
	
}

void DeviceContext::tempPresent()
{

	vk::SubmitInfo submitInfo;

	vk::Semaphore waitSemaphores[] = { imageAvailableSemaphore };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &swapchain.commandBuffers[finalCommandBufferIndex];

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
	presentInfo.pImageIndices = &finalCommandBufferIndex;

	presentQueue.presentKHR(presentInfo);
	presentQueue.waitIdle();

}

void DeviceContext::executeCommandQueue()
{


	vk::CommandBuffer commandBuffer;

	if (mode == DEVICE_MODE::HEADLESS)
		commandBuffer = renderPassCommandBuffer;
	else
	{
		uint32_t imageIndex = device.acquireNextImageKHR(swapchain.swapchain, UINT64_MAX, imageAvailableSemaphore, vk::Fence()).value;
		commandBuffer = swapchain.commandBuffers[imageIndex];
		finalCommandBufferIndex = imageIndex;
	}
	
	vk::SubmitInfo submitInfo;


	vk::Semaphore waitSemaphores[] = { imageAvailableSemaphore };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	if (mode == DEVICE_MODE::WINDOW)
	{
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
	}
	
	
	graphicsQueue.submit(1, &submitInfo, {});

	graphicsQueue.waitIdle();

}

void DeviceContext::submitMesh(Mesh * mesh)
{
	renderQueue[mesh->getTechnique()].push_back(mesh);
}

void DeviceContext::renderGeometry()
{

	//maybe only do this once? :)
	for (auto& queueElement : renderQueue)
	{
		queueElement.first->bindMaterial();
		Camera::getInstance()->bindCamera(this, queueElement.first->getDescriptionSets()[0]);
	}

	if (mode == DEVICE_MODE::HEADLESS)
	{
		

		for (auto& queueElement : renderQueue)
		{
			queueElement.first->bindPipeline(renderPassCommandBuffer);
			queueElement.first->bindDescriptorSet(renderPassCommandBuffer);
			for (Mesh* mesh : queueElement.second)
			{
				mesh->draw(renderPassCommandBuffer);
			}
		}
		renderPassCommandBuffer.endRenderPass();

		//Transfer texture
		renderPassCommandBuffer.executeCommands(1, &transferToCpuCommandBuffer);

		renderPassCommandBuffer.end();
	}
	else
	{
		for (size_t i = 0; i < swapchain.commandBuffers.size(); i++)
		{
			vk::CommandBuffer& commandBuffer = swapchain.commandBuffers[i];

			for (auto& queueElement : renderQueue)
			{
				queueElement.first->bindPipeline(commandBuffer);
				queueElement.first->bindDescriptorSet(commandBuffer);
				for (Mesh* mesh : queueElement.second)
				{
					mesh->draw(commandBuffer);
				}
			}

			commandBuffer.endRenderPass();
			commandBuffer.end();
		}
	}
	renderQueue.clear();
	
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

DeviceContext::SecondaryDeviceTexturePair * DeviceContext::getTexturePair(DeviceContext * deviceContext)
{
	return secondaryDeviceTextures.at(deviceContext);
}

std::map<DeviceContext*, DeviceContext::SecondaryDeviceTexturePair*> DeviceContext::getTexturePairs()
{
	return secondaryDeviceTextures;
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


	vk::AttachmentDescription depthAttachment;
	depthAttachment.format = getMainDevice()->swapchain.depthImage->getFormat(); //maybe hardcode?
	depthAttachment.samples = vk::SampleCountFlagBits::e1;
	depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	depthAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eClear;
	depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eStore;
	depthAttachment.initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
	//If we are the main GPU we will set our final layout to an image to be used for input in the next pass.
	//If we are a slave GPU we will set our layout to an image to be used as a source for a copy operation.
	switch (mode)
	{
		case DEVICE_MODE::WINDOW:
			colorAttachment.finalLayout = vk::ImageLayout::eTransferDstOptimal;
			depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;
			break;
		case DEVICE_MODE::HEADLESS:
			colorAttachment.finalLayout = vk::ImageLayout::eTransferSrcOptimal;
			depthAttachment.finalLayout = vk::ImageLayout::eTransferSrcOptimal;
			break;
	}
	

	vk::AttachmentReference colorAttachmentRef;
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

	vk::AttachmentReference depthAttachmentRef;
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	vk::SubpassDescription subpass;
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	vk::SubpassDependency dependecy;
	dependecy.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependecy.dstSubpass = 0;
	dependecy.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependecy.srcAccessMask = {};

	dependecy.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependecy.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

	std::vector<vk::AttachmentDescription> attachments = { colorAttachment, depthAttachment };

	vk::RenderPassCreateInfo renderPassInfo;
	renderPassInfo.attachmentCount = attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependecy;

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


	vk::AttachmentDescription PresentDepthAttachment;
	PresentDepthAttachment.format = swapchain.depthImage->getFormat(); //maybe hardcode?
	PresentDepthAttachment.samples = vk::SampleCountFlagBits::e1;
	PresentDepthAttachment.loadOp = vk::AttachmentLoadOp::eLoad;
	PresentDepthAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	PresentDepthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eLoad;
	PresentDepthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eStore;
	PresentDepthAttachment.initialLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;
	PresentDepthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
	
	std::vector<vk::AttachmentDescription> attatchments = { colorAttachment, PresentDepthAttachment };
	std::vector<vk::AttachmentReference> inputAttatchmentRefs;


	vk::AttachmentReference colorAttachmentRef;
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

	vk::AttachmentReference depthInputAttachmentRef;
	depthInputAttachmentRef.attachment = 1;
	depthInputAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	inputAttatchmentRefs.push_back(depthInputAttachmentRef);

	int i = 2;
	for (auto& secondaryTexture : secondaryDeviceTextures)
	{
		
		vk::AttachmentDescription inputAttatchment;
		inputAttatchment.format = swapchain.imageFormat; //maybe hardcode?
		inputAttatchment.samples = vk::SampleCountFlagBits::e1;
		inputAttatchment.loadOp = vk::AttachmentLoadOp::eLoad;
		inputAttatchment.storeOp = vk::AttachmentStoreOp::eStore;
		inputAttatchment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		inputAttatchment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		inputAttatchment.initialLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		inputAttatchment.finalLayout = vk::ImageLayout::eTransferDstOptimal;

		attatchments.push_back(inputAttatchment);

		vk::AttachmentReference inputAttatchmentRef;
		inputAttatchmentRef.attachment = i;
		inputAttatchmentRef.layout = vk::ImageLayout::eShaderReadOnlyOptimal;
		inputAttatchmentRefs.push_back(inputAttatchmentRef);


		vk::AttachmentDescription depthAttachment;
		depthAttachment.format = swapchain.depthImage->getFormat(); //maybe hardcode?
		depthAttachment.samples = vk::SampleCountFlagBits::e1;
		depthAttachment.loadOp = vk::AttachmentLoadOp::eLoad;
		depthAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eLoad;
		depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eStore;
		depthAttachment.initialLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;
		depthAttachment.finalLayout = vk::ImageLayout::eTransferDstOptimal;

		attatchments.push_back(depthAttachment);

		vk::AttachmentReference depthAttachmentRef;
		depthAttachmentRef.attachment = i+1;
		depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;
		inputAttatchmentRefs.push_back(depthAttachmentRef);

		i+=2;
	}


	vk::SubpassDescription subpass;
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.inputAttachmentCount = inputAttatchmentRefs.size();
	subpass.pInputAttachments = inputAttatchmentRefs.data();

	vk::SubpassDependency dependecy;
	dependecy.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependecy.dstSubpass = 0;
	dependecy.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependecy.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

	dependecy.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
	dependecy.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

	vk::RenderPassCreateInfo renderPassInfo;
	renderPassInfo.attachmentCount = attatchments.size();
	renderPassInfo.pAttachments = attatchments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependecy;

	presentRenderPass = device.createRenderPass(renderPassInfo);
}

void DeviceContext::createFrameBuffers()
{
	int frameBufferCount = swapchain.imageViews.size();
	swapchain.framebuffers.resize(frameBufferCount);
	swapchain.finalframebuffers.resize(frameBufferCount);

	//create an image view for the output to the swap texture.
	for (size_t i = 0; i < frameBufferCount; i++) {
		std::vector<vk::ImageView> attachments = { swapchain.imageViews[i], swapchain.depthImage->getImageView() };
		std::vector<vk::ImageView> finalFrameBufferattachments = { swapchain.imageViews[i], swapchain.depthImage->getImageView() };
		for (auto & secondaryTextures : secondaryDeviceTextures)
		{
			finalFrameBufferattachments.push_back(secondaryTextures.second->renderTexture->getImageView());
			finalFrameBufferattachments.push_back(secondaryTextures.second->renderDepthTexture->getImageView());
		}
			
		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapchain.extent.width;
		framebufferInfo.height = swapchain.extent.height;
		framebufferInfo.layers = 1;

		swapchain.framebuffers[i] = device.createFramebuffer(framebufferInfo);

		framebufferInfo.renderPass = presentRenderPass;
		framebufferInfo.attachmentCount = finalFrameBufferattachments.size();
		framebufferInfo.pAttachments = finalFrameBufferattachments.data();

		swapchain.finalframebuffers[i] = device.createFramebuffer(framebufferInfo);
	}

}

void DeviceContext::createSecondaryDeviceTexture(DeviceContext * deviceContext)
{
	SecondaryDeviceTexturePair* pair = new SecondaryDeviceTexturePair();
	pair->renderTexture = new RenderTexture(this,
		getMainDevice()->swapchain.extent.width,
		getMainDevice()->swapchain.extent.height,
		getMainDevice()->swapchain.imageFormat);

	pair->targetTexture = new Texture(this,
		getMainDevice()->swapchain.extent.width,
		getMainDevice()->swapchain.extent.height,
		getMainDevice()->swapchain.imageFormat,
		vk::ImageLayout::eUndefined,
		vk::ImageTiling::eLinear,
		vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostCached);

	pair->renderDepthTexture = new Texture(this,
		getMainDevice()->swapchain.extent.width,
		getMainDevice()->swapchain.extent.height,
		getMainDevice()->swapchain.depthImage->getFormat(),
		vk::ImageLayout::eUndefined,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eDepthStencilAttachment,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		vk::ImageAspectFlagBits::eDepth);


	pair->targetDepthBuffer = new Buffer(
		this,
		getMainDevice()->swapchain.extent.width * getMainDevice()->swapchain.extent.height * 4,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc,
		vk::SharingMode::eExclusive,
		vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCached);



	vk::ImageLayout renderTextureLayout = mode == DEVICE_MODE::HEADLESS ? vk::ImageLayout::eGeneral : vk::ImageLayout::eTransferDstOptimal;
	vk::ImageLayout renderDepthTextureLayout = mode == DEVICE_MODE::HEADLESS ? vk::ImageLayout::eDepthStencilAttachmentOptimal : vk::ImageLayout::eTransferDstOptimal;
	executeSingleTimeQueue(
		[pair, renderTextureLayout, renderDepthTextureLayout](vk::CommandBuffer commandBuffer)
	{
		VulkanHelpers::cmdTransitionLayout(
			commandBuffer,
			pair->renderDepthTexture->getImage(),
			vk::ImageLayout::eUndefined, renderDepthTextureLayout,
			{}, vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
			vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eEarlyFragmentTests, vk::ImageAspectFlagBits::eDepth);

		VulkanHelpers::cmdTransitionLayout(
			commandBuffer,
			pair->renderTexture->getImage(),
			vk::ImageLayout::eUndefined, renderTextureLayout,
			{}, vk::AccessFlagBits::eMemoryWrite,
			vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::ImageAspectFlagBits::eColor);

		VulkanHelpers::cmdTransitionLayout(
			commandBuffer,
			pair->targetTexture->getImage(),
			vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral,
			{}, vk::AccessFlagBits::eMemoryWrite,
			vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::ImageAspectFlagBits::eColor);
	});

	secondaryDeviceTextures.insert(std::make_pair(deviceContext, pair));

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
	createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;


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

	//swapchain depth texture
	swapchain.depthImage = new Texture(
		this,
		swapchain.extent.width, swapchain.extent.height,
		vk::Format::eD32Sfloat,
		vk::ImageLayout::eUndefined, vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eInputAttachment,
		vk::MemoryPropertyFlagBits::eDeviceLocal, vk::ImageAspectFlagBits::eDepth);


	//Transition depth image layout
	executeSingleTimeQueue(
		[this](vk::CommandBuffer commandBuffer)
	{

		VulkanHelpers::cmdTransitionLayout(
			commandBuffer,
			this->getSwapchain().depthImage->getImage(),
			vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal,
			{}, vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
			vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eEarlyFragmentTests, vk::ImageAspectFlagBits::eDepth);

	});
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
	
	vk::CommandBufferInheritanceInfo inheritanceInfo;
	inheritanceInfo.framebuffer = nullptr;
	inheritanceInfo.occlusionQueryEnable = VK_FALSE;

	if (mode == DEVICE_MODE::HEADLESS)
	{
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1;
		renderPassCommandBuffer = device.allocateCommandBuffers(allocInfo)[0];

		allocInfo.level = vk::CommandBufferLevel::eSecondary;
		allocInfo.commandBufferCount = 1;
		transferToCpuCommandBuffer = device.allocateCommandBuffers(allocInfo)[0];

		vk::CommandBufferBeginInfo beginInfo;
		beginInfo.pInheritanceInfo = &inheritanceInfo;
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
		transferToCpuCommandBuffer.begin(beginInfo);

		Texture* targetTexture = secondaryDeviceTextures[this]->targetTexture;
		Texture* renderDepthTexture = secondaryDeviceTextures[this]->renderDepthTexture;
		RenderTexture* renderTexture = secondaryDeviceTextures[this]->renderTexture;
		Buffer* targetDepthBuffer = secondaryDeviceTextures[this]->targetDepthBuffer;


		//render
		VulkanHelpers::cmdTransitionLayout(
			transferToCpuCommandBuffer,
			targetTexture->getImage(),
			vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferDstOptimal,
			{}, vk::AccessFlagBits::eTransferWrite,
			vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::ImageAspectFlagBits::eColor);

		VulkanHelpers::cmdCopyImageSimple(transferToCpuCommandBuffer,
			renderTexture->getImage(), vk::ImageLayout::eTransferSrcOptimal,
			targetTexture->getImage(), vk::ImageLayout::eTransferDstOptimal,
			renderTexture->getExtends().width, renderTexture->getExtends().height);


		VulkanHelpers::cmdTransitionLayout(
			transferToCpuCommandBuffer,
			targetTexture->getImage(),
			vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eGeneral,
			vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eTransferRead,
			vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::ImageAspectFlagBits::eColor);

		//depth buffer

		VulkanHelpers::cmdCopyImageToBuffer(
			transferToCpuCommandBuffer,
			renderDepthTexture->getImage(), targetDepthBuffer->getBuffer(),
			vk::ImageLayout::eTransferSrcOptimal,
			renderDepthTexture->getExtends().width, renderDepthTexture->getExtends().height,
			vk::ImageAspectFlagBits::eDepth);

		VulkanHelpers::cmdTransitionLayout(
			transferToCpuCommandBuffer,
			renderDepthTexture->getImage(),
			vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eDepthStencilAttachmentOptimal,
			vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
			vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
			vk::PipelineStageFlagBits::eEarlyFragmentTests, vk::PipelineStageFlagBits::eEarlyFragmentTests, vk::ImageAspectFlagBits::eDepth);

		transferToCpuCommandBuffer.end();
	}
	else
	{
		swapchain.commandBuffers.resize(swapchain.framebuffers.size());
		allocInfo.commandBufferCount = (uint32_t)swapchain.commandBuffers.size();
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		swapchain.commandBuffers = device.allocateCommandBuffers(allocInfo);

		allocInfo.commandBufferCount = 1;
		allocInfo.level = vk::CommandBufferLevel::eSecondary;

		combineDaQCommandBuffer = device.allocateCommandBuffers(allocInfo)[0];



		vk::CommandBufferBeginInfo beginInfo;
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
		beginInfo.pInheritanceInfo = &inheritanceInfo;
		combineDaQCommandBuffer.begin(beginInfo);

		for (auto& texturePairs : secondaryDeviceTextures)
		{
			Texture* targetTexture = texturePairs.second->targetTexture;
			RenderTexture* renderTexture = texturePairs.second->renderTexture;
			Texture* renderDepthTexture = texturePairs.second->renderDepthTexture;
			Buffer* targetDepthBuffer = texturePairs.second->targetDepthBuffer;
			//Render
			VulkanHelpers::cmdTransitionLayout(
				combineDaQCommandBuffer,
				targetTexture->getImage(),
				vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal,
				vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eMemoryRead,
				vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::ImageAspectFlagBits::eColor);


			VulkanHelpers::cmdBlitSimple(
				combineDaQCommandBuffer,
				targetTexture->getImage(), vk::ImageLayout::eTransferSrcOptimal,
				renderTexture->getImage(), vk::ImageLayout::eTransferDstOptimal,
				swapchain.extent.width, swapchain.extent.height,
				vk::Filter::eNearest);

			VulkanHelpers::cmdTransitionLayout(
				combineDaQCommandBuffer,
				renderTexture->getImage(),
				vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
				vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eMemoryRead,
				vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::ImageAspectFlagBits::eColor);

			VulkanHelpers::cmdTransitionLayout(
				combineDaQCommandBuffer,
				targetTexture->getImage(),
				vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eGeneral,
				vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eMemoryRead,
				vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::ImageAspectFlagBits::eColor);

			//buffer
			VulkanHelpers::cmdCopyBufferToImage(
				combineDaQCommandBuffer,
				targetDepthBuffer->getBuffer(),
				renderDepthTexture->getImage(), vk::ImageLayout::eTransferDstOptimal,
				swapchain.extent.width, swapchain.extent.height, vk::ImageAspectFlagBits::eDepth);

			VulkanHelpers::cmdTransitionLayout(
				combineDaQCommandBuffer,
				renderDepthTexture->getImage(),
				vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eDepthStencilReadOnlyOptimal,
				vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eMemoryRead,
				vk::PipelineStageFlagBits::eEarlyFragmentTests, vk::PipelineStageFlagBits::eEarlyFragmentTests, vk::ImageAspectFlagBits::eDepth);
		}
		combineDaQCommandBuffer.end();

	}
	
}

void DeviceContext::createSemaphores()
{
	//TODO more things
	vk::SemaphoreCreateInfo semaphoreInfo;

	imageAvailableSemaphore = device.createSemaphore(semaphoreInfo);
	renderFinishedSemaphore = device.createSemaphore(semaphoreInfo);
}

void DeviceContext::startFinalRenderPass()
{
	if (deviceGroup->getGroupSize() == 1)
		return;

	vk::CommandBufferBeginInfo beginInfo;

	beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

	vk::RenderPassBeginInfo finalPassInfo;
	finalPassInfo.renderPass = presentRenderPass;
	finalPassInfo.renderArea.offset = { 0, 0 };
	finalPassInfo.clearValueCount = 0;

	vk::CommandBuffer& commandBuffer = swapchain.commandBuffers[finalCommandBufferIndex];
	commandBuffer.begin(beginInfo);


	finalPassInfo.framebuffer = swapchain.finalframebuffers[finalCommandBufferIndex];
	finalPassInfo.renderArea.extent = swapchain.extent;
				
	commandBuffer.executeCommands(1, &combineDaQCommandBuffer);

	commandBuffer.beginRenderPass(finalPassInfo, vk::SubpassContents::eInline);
	combineTechnique->bindPipeline(commandBuffer);
	combineTechnique->bindDescriptorSet(commandBuffer);
	commandBuffer.draw(6, 1, 0, 0);

	commandBuffer.endRenderPass();
	commandBuffer.end();
}

void DeviceContext::createSecondaryDeviceFramebuffer()
{

	RenderTexture* renderTexture = secondaryDeviceTextures[this]->renderTexture;
	Texture* renderDepthTexture = secondaryDeviceTextures[this]->renderDepthTexture;

	vk::ImageView attachments[] = { renderTexture->getImageView(),  renderDepthTexture->getImageView()};

	vk::FramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = 2;
	framebufferInfo.pAttachments = attachments;
	framebufferInfo.width = renderTexture->getExtends().width;
	framebufferInfo.height = renderTexture->getExtends().height;
	framebufferInfo.layers = 1;

	renderTextureFrameBuffer = device.createFramebuffer(framebufferInfo);
}

void DeviceContext::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies();

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

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

	if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
	{
		return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
	}

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
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
		if (availablePresentMode == vk::PresentModeKHR::eMailbox)
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
