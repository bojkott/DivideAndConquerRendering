#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL_vulkan.h>
#include "Common.h"
#include "Window.h"


class Renderer
{
private:
	vk::Instance instance;
	vk::DebugReportCallbackEXT callback;
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

public:
	Renderer();
	~Renderer();


private:
	#define FAILED(x) x != vk::Result::eSuccess

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif


	bool checkValidationLayerSupport();
	void createInstance();
	void setupDebugCallBack();
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