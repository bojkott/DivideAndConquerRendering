#pragma once
#include <map>
#include <vulkan\vulkan.hpp>
#include <glm\glm.hpp>
#include "DeviceContext.h"
#include "vkGroups.h"

class Camera
{
public:
	struct UniformBufferObject
	{
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec4 pos;
		int deviceId;
	};



private:
	vkGroups::UniformBufferGroup bufferGroup;
	UniformBufferObject cameraObject;
	static Camera* instance;

	glm::vec3 cameraPos = glm::vec3(0.0f, 100.0f, 0.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

public:
	static Camera* getInstance(float x = 0, float y = 0, float z = 0);
	void bindCamera(DeviceContext* context, vk::DescriptorSet descSet);
	void update(float dt);

	~Camera();
	Camera() {}; // Fuck you map




private:
	Camera(float x = 0, float y = 0, float z = 0);
	void updateCameraBuffer();

};