#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL_vulkan.h>
#include "Window.h"
#include "DeviceGroup.h"

class Technique;
class Renderer
{
public:
	#ifdef _DEBUG
		static const bool enableValidationLayers = true;
	#else
		static const bool enableValidationLayers = false;
	#endif
	static const std::vector<const char*> validationLayers;
	static DeviceGroup deviceGroup;
private:
	Renderer();
	#define FAILED(x) x != vk::Result::eSuccess

	
	vk::SurfaceKHR surface;
	vk::Instance instance;
	vk::DebugReportCallbackEXT callback;
	
	Technique* daQCombineTechnique;

	double transferTime = 0.0f;

	bool slaveDevicesEnabled = true;
	bool autoLoadBalance = false;

	static Renderer* RenderInstance;

public:
	static Renderer* getInstance();
	double getTransferTime();
	~Renderer();
	
	void render();

	void toggleSlaveDevices(bool value);
	bool getSlaveDevicesEnabled();
private:


	void executeGeometryPass(DeviceContext* device);
	void balanceDeviceTime(std::map<DeviceContext*, float>& times);


	bool checkValidationLayerSupport();
	void createInstance();
	void setupDebugCallBack();
	void createSurface();
	void setupDeviceGroup();
	bool isDeviceSuitable(const vk::PhysicalDevice& device);
	void pickMaster(vk::PhysicalDevice& master, std::vector<vk::PhysicalDevice>& devices);
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