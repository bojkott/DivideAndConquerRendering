#pragma once
#include <map>
#include <vulkan\vulkan.hpp>
#include <glm\glm.hpp>
#include "DeviceContext.h"
#include "Buffer.h"

class Camera
{
public:
	struct UniformBufferObject
	{
		glm::mat4 view;
		glm::mat4 proj;
	};



private:
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::Device* device;
	Buffer cameraBuffer;
	UniformBufferObject cameraObject;
	static std::map<DeviceContext*, Camera> cameraDict;

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

public:
	vk::DescriptorSetLayout* getDescriptorSetLayout();
	static Camera getInstance(DeviceContext* context, float x = 0, float y = 0, float z = 0);
	~Camera();
	Camera() {}; // Fuck you map
	void update(float dt);


private:
	Camera(DeviceContext* context, float x = 0, float y = 0, float z = 0);
	void updateCameraBuffer();

};