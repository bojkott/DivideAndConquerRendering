#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL_vulkan.h>
#include "Common.h"
#include "Window.h"


class Renderer
{
public:
	#ifdef _DEBUG
		static const bool enableValidationLayers = true;
	#else
		static const bool enableValidationLayers = false;
	#endif
	static const std::vector<const char*> validationLayers;
private:

	#define FAILED(x) x != vk::Result::eSuccess

	

	vk::Instance instance;
	vk::DebugReportCallbackEXT callback;
	

public:
	Renderer();
	~Renderer();


private:

	bool checkValidationLayerSupport();
	void createInstance();
	void setupDebugCallBack();
	void setupDeviceGroup();
	bool isDeviceSuitable(const vk::PhysicalDevice& device);
	void arrangeGroup(std::vector<vk::PhysicalDevice>& devices);
	std::vector<const char*> getRequiredExtensions();

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
		vk::DebugReportFlagsEXT flags,
		vk::DebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData);

	vk::Result CreateDebugReportCallbackEXT(vk::Instance instance, 
		const vk::DebugReportCallbackCreateInfoEXT* pCreateInfo,
		const vk::AllocationCallbacks* pAllocator, 
		vk::DebugReportCallbackEXT* pCallback);

	void DestroyDebugReportCallbackEXT(vk::Instance instance,
		vk::DebugReportCallbackEXT callback,
		const vk::AllocationCallbacks* pAllocator);
};