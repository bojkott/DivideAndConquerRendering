#pragma once
#include <vulkan\vulkan.hpp>
#include <glm\glm.hpp>
#include "DeviceContext.h"
#include "Buffer.h"

class Camera
{
public:
	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	static vk::DescriptorSetLayout descriptorSetLayout;
	static vk::Device* device;
	static Buffer cameraBuffer;
	static UniformBufferObject cameraObject;

private:


public:
	static vk::DescriptorSetLayout* getDescriptorSetLayout();
	static void Init(DeviceContext* context, float x = 0, float y = 0, float z = 0);
	static void Destroy();
	

private:



};