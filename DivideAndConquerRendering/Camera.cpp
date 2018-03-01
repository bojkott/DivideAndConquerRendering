#include "Camera.h"
#include "Renderer.h"

vk::DescriptorSetLayout Camera::descriptorSetLayout;
vk::Device* Camera::device = nullptr;
Buffer Camera::cameraBuffer;
Camera::UniformBufferObject Camera::cameraObject;


vk::DescriptorSetLayout* Camera::getDescriptorSetLayout()
{
	return &descriptorSetLayout;
}

void Camera::Init(DeviceContext* context, float x, float y, float z)  
{
	cameraBuffer = Buffer(context,
		sizeof(UniformBufferObject), vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	vk::DescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
	uboLayoutBinding.descriptorCount = 1;

	uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	vk::DescriptorSetLayoutCreateInfo layout = {};
	layout.bindingCount = 1;
	layout.pBindings = &uboLayoutBinding;

	context->getDevice().createDescriptorSetLayout(&layout, nullptr, &descriptorSetLayout);

	cameraObject.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	cameraObject.model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	cameraObject.proj = glm::perspective(glm::radians(45.0f), (float) context->getSwapchain().extent.width / (float) context->getSwapchain().extent.height,
		0.1f, 10.0f);

	cameraObject.proj[1][1] *= -1;

	cameraBuffer.setData(&cameraObject);
}

void Camera::Destroy()
{
	if (device != nullptr)
		device->destroyDescriptorSetLayout(descriptorSetLayout);
}
