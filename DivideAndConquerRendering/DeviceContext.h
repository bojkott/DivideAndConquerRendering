#pragma once

#include <vulkan\vulkan.hpp>
#include <SDL.h>
#include <memory>
#include <functional>
#include <map>

class Buffer;
class RenderTexture;
class Texture;
class Mesh;
class DeviceGroup;
class Technique;
class DeviceContext {
public:
	enum class DEVICE_MODE {WINDOW, HEADLESS};
	enum class RENDERPASS_TYPE {Final, Standard};	

private:

	DeviceGroup* deviceGroup;
	DEVICE_MODE mode;

	
	vk::PhysicalDevice physicalDevice;
	vk::Device device;
	vk::CommandPool commandPool;
	vk::Queue graphicsQueue;
	vk::Queue presentQueue;
	

	struct QueueFamilyIndices
	{
		int graphicsFamily = -1;
		bool isComplete()
		{
			return graphicsFamily >= 0;
		}
	};



public:
	DeviceContext(DeviceGroup* group, vk::Instance& instance, vk::PhysicalDevice physicalDevice);
	~DeviceContext();

	vk::Device& getDevice();



	void executeSingleTimeQueue(std::function< void (vk::CommandBuffer)> commands);

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
private:
	void createDevice(vk::Instance& instance);

	void createCommandPool();
	QueueFamilyIndices DeviceContext::findQueueFamilies();



	DeviceContext* getMainDevice();

};