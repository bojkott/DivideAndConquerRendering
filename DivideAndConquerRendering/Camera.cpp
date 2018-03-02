#include "Camera.h"
#include "Renderer.h"
#include "Window.h"

std::map<DeviceContext*, Camera> Camera::cameraDict;


vk::DescriptorSetLayout* Camera::getDescriptorSetLayout()
{
	return &descriptorSetLayout;
}

Camera Camera::getInstance(DeviceContext* context, float x, float y, float z)  
{
	if (cameraDict.find(context) == cameraDict.end())
	{
		cameraDict[context] = Camera(context, x, y, z);
	}
	return cameraDict[context];
}

Camera::~Camera()
{
	device->destroyDescriptorSetLayout(descriptorSetLayout);
}

void Camera::update(float dt)
{
	int x, y;

	SDL_GetRelativeMouseState(&x, &y);
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	float cameraSpeed = 0.05f * dt;


	// Fucking keyboard n shit
	if (state[SDL_SCANCODE_W])
		cameraPos += cameraSpeed * cameraFront;

	if (state[SDL_SCANCODE_S])
		cameraPos -= cameraSpeed * cameraFront;

	if (state[SDL_SCANCODE_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (state[SDL_SCANCODE_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;


	// Fucking mouse fuck
	float sensitivity = 0.05;
    y *= sensitivity;
    x *= sensitivity;

	static float yaw = 0;
	static float pitch = 0;
	yaw += x;
	pitch += y;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
	

	cameraObject.view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

Camera::Camera(DeviceContext * context, float x, float y, float z)
{
	cameraBuffer = std::move(Buffer(context,
		sizeof(UniformBufferObject), vk::BufferUsageFlagBits::eUniformBuffer, vk::SharingMode::eExclusive,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCached));
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
	
	// TODO: sätt den till faktiska mitten
	SDL_WarpMouseInWindow(Window::window, 400, 300);


	cameraObject.proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	cameraObject.proj[1][1] *= -1;

	cameraBuffer.setData(&cameraObject);
}

void Camera::updateCameraBuffer()
{

}

