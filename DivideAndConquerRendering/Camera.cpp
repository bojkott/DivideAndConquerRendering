#include "Camera.h"
#include "Renderer.h"
#include "Window.h"
#include "UniformBuffer.h"
#include "Material.h"
Camera* Camera::instance;

Camera* Camera::getInstance( float x, float y, float z)  
{
	if (instance == nullptr)
	{
		instance = new Camera(x, y, z);
	}
	return instance;
}

void Camera::bindCamera(DeviceContext * context, vk::DescriptorSet descSet)
{
	bufferGroup.sets[context]->bind(PER_CAMERA_BINDING, descSet);
}

Camera::~Camera()
{
	delete instance;
}

void Camera::update(float dt)
{
	int x, y;
	SDL_GetRelativeMouseState(&x, &y);
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	float cameraSpeed = 100.0f * dt;


	// Fucking keyboard n shit
	if (state[SDL_SCANCODE_W])
		cameraPos += cameraSpeed * cameraFront;

	if (state[SDL_SCANCODE_S])
		cameraPos -= cameraSpeed * cameraFront;

	if (state[SDL_SCANCODE_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (state[SDL_SCANCODE_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (state[SDL_SCANCODE_Q])
		cameraPos += cameraSpeed * cameraUp;

	if (state[SDL_SCANCODE_E])
		cameraPos -= cameraSpeed * cameraUp;

	// Fucking mouse fuck
	float sensitivity = 0.5 * dt;
    y *= -sensitivity;
    x *= sensitivity;

	static float yaw = 95;
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
	updateCameraBuffer();
}

Camera::Camera(float x, float y, float z)
{
	Renderer::deviceGroup.createUniformBufferGroup(sizeof(UniformBufferObject), bufferGroup);

	SDL_WarpMouseInWindow(Window::window, 400, 300);

	cameraObject.proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 10000.0f);
	cameraObject.view = glm::lookAt(cameraPos, cameraFront, cameraUp);

	cameraObject.proj[1][1] *= -1;

	for (auto& pair : bufferGroup.sets)
		pair.second->setData(&cameraObject);
}

void Camera::updateCameraBuffer()
{
	int deviceId = 0;
	for (auto& pair : bufferGroup.sets)
	{
		cameraObject.deviceId = deviceId;
		pair.second->setData(&cameraObject);
		deviceId++;
	}
		
}

