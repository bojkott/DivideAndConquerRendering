#include "Renderer.h"
#include "DeviceContext.h"
#include "Texture.h"
#include <iostream>
#include "materials\DaQCombineMaterial.h"
#include "Technique.h"
#include "Buffer.h"
#include <thread>
#include <future>
#include "Camera.h"

const std::vector<const char*> Renderer::validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

#include "Model.h"

Renderer* Renderer::RenderInstance = nullptr;
DeviceGroup Renderer::deviceGroup;

Renderer::Renderer()
{
	createInstance();
#ifdef _DEBUG
	setupDebugCallBack();
#endif
	createSurface();
	setupDeviceGroup();

	daQCombineTechnique = Technique::createOrGetTechnique(deviceGroup.getMainDevice(), new DaQCombineMaterial(), new RenderState(vk::FrontFace::eClockwise));
	deviceGroup.getMainDevice()->setCombineTechnique(daQCombineTechnique);

}

Renderer * Renderer::getInstance()
{
	if (RenderInstance)
		return RenderInstance;
	else
		RenderInstance = new Renderer();
}

double Renderer::getTransferTime()
{
	return transferTime;
}

Renderer::~Renderer()
{
	deviceGroup.~DeviceGroup();
	instance.destroySurfaceKHR(surface, nullptr);
	DestroyDebugReportCallbackEXT(instance, callback, nullptr);
	instance.destroy();
}

void Renderer::render()
{
	DeviceContext* mainDevice = deviceGroup.getMainDevice();

	std::map<DeviceContext*, float> deviceTimes;

	if (slaveDevicesEnabled)
	{
		for (auto& slaveDevices : mainDevice->getTexturePairs())
		{
			DeviceContext* device = slaveDevices.first;
			executeGeometryPass(device);
		}
	}

	executeGeometryPass(mainDevice);
			
	transferTime = 0;

	if (slaveDevicesEnabled)
	{
		for (auto& slaveDeviceTarget : mainDevice->getTexturePairs())
		{
			DeviceContext* slaveDevice = slaveDeviceTarget.first;
			slaveDevice->waitForGeometry();
			float slaveTime = slaveDevice->getTimeTaken();
			deviceTimes[slaveDevice] = slaveTime;
			Uint32 transferStart = SDL_GetPerformanceCounter();
			slaveDeviceTarget.first->getTexturePair(slaveDeviceTarget.first)->targetTexture->transferTextureTo(*slaveDeviceTarget.second->targetTexture);
			slaveDeviceTarget.first->getTexturePair(slaveDeviceTarget.first)->targetDepthBuffer->transferBufferTo(*slaveDeviceTarget.second->targetDepthBuffer);
			Uint32 transferEnd = SDL_GetPerformanceCounter();

			float deviceTransferTime = (float)((transferEnd - transferStart) * 1000.0 / SDL_GetPerformanceFrequency());
			deviceTimes[slaveDevice] += deviceTransferTime;
			transferTime += deviceTransferTime;
		}
	}

	
	

	mainDevice->waitForGeometry();
	float mainDeviceGeometryTime = mainDevice->getTimeTaken();
	deviceTimes[mainDevice] = mainDeviceGeometryTime;


	if (slaveDevicesEnabled)
		mainDevice->startFinalRenderPass();
	mainDevice->tempPresent(slaveDevicesEnabled);
	mainDevice->waitForMainDevice();
	float combineTime = mainDevice->getTimeTaken();

	balanceDeviceTime(deviceTimes);
	
	std::cout << "combine time: " << combineTime << std::endl;
}

void Renderer::toggleSlaveDevices(bool value)
{
	slaveDevicesEnabled = value;
}

bool Renderer::getSlaveDevicesEnabled()
{
	return slaveDevicesEnabled;
}

void Renderer::executeGeometryPass(DeviceContext* device)
{
	device->clearBuffer(0, 0, 0, 1);
	device->renderGeometry();
	device->executeCommandQueue();
}

void Renderer::balanceDeviceTime(std::map<DeviceContext*, float>& times)
{
	
	if (!autoLoadBalance && times.size() == 2)
	{
		const Uint8 *state = Camera::getInstance()->state;
		float change = 0;
		if (state[SDL_SCANCODE_LEFT])
		{
			change = 0.05f;
		}
		else if (state[SDL_SCANCODE_RIGHT])
			change = -0.05;

		for (auto& time : times)
		{
			time.first->setLoadPercentage(time.first->getLoadPercentage() + change);
			change *= -1;
		}
	}

	int deviceIndex = 0;
	for (auto& time : times)
	{
		if (autoLoadBalance)
		{
			for (auto& time2 : times)
			{
				if (time.first != time2.first)
				{
					if (time.second > time2.second)
					{
						float time1Load = time.first->getLoadPercentage();
						float time2Load = time2.first->getLoadPercentage();
						time.first->setLoadPercentage(time1Load - 0.05f);
						time2.first->setLoadPercentage(time2Load + 0.05f);
					}
				}
		}

		}
		std::string type = time.first == deviceGroup.getMainDevice() ? "master" : "slave";
		std::cout << "(" << type << ") device " << deviceIndex << ": " << time.second << "(" << time.first->getLoadPercentage() << ") ";
		deviceIndex++;
	}
	std::cout  << std::endl;
}

bool Renderer::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vk::enumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<vk::LayerProperties> availableLayers(layerCount);

	vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const auto& layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

void Renderer::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport())
		throw std::runtime_error("validation layers requested but not available");

	vk::ApplicationInfo appInfo;
	appInfo.engineVersion = 0U;
	appInfo.pEngineName = "Dank memes";
	appInfo.pApplicationName = "DivideAndConquer:Zero overhead";
	appInfo.apiVersion = VK_API_VERSION_1_0;




	vk::InstanceCreateInfo createInfo;
	vk::DebugReportCallbackEXT callback;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}
	vk::Result result = vk::createInstance(&createInfo, nullptr, &instance);
	if (FAILED(result))
	{
		throw std::runtime_error("failed to create instance");
	}
}

void Renderer::setupDebugCallBack()
{
	vk::DebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT) debugCallback;
	createInfo.flags = vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning; 

	if (FAILED(CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback)))
	{
		throw std::runtime_error("failed to set up debug callback!");
	}
	
}

void Renderer::createSurface()
{
	if (SDL_Vulkan_CreateSurface(Window::window, instance, (VkSurfaceKHR*)&surface) != SDL_TRUE)
	{ 
		throw std::runtime_error("Failed to create surface");
	}
}

void Renderer::setupDeviceGroup()
{

	std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
	
	for (int i = 0; i < physicalDevices.size(); i++)
	{
		if (!isDeviceSuitable(physicalDevices[i]))
		{
			physicalDevices.erase(physicalDevices.begin()+i);
			i -= 1;
		}
	}
	vk::PhysicalDevice master;
	pickMaster(master, physicalDevices);



	deviceGroup.addDevice(instance, master, surface);
	for (auto& slave = physicalDevices.begin(); slave != physicalDevices.end(); slave++)
		deviceGroup.addDevice(instance, *slave);

	deviceGroup.initDevices();
}

bool Renderer::isDeviceSuitable(const vk::PhysicalDevice & device)
{
	vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
	vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

	

	return deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
	return true; // all is welcome, for now
}

void Renderer::pickMaster(vk::PhysicalDevice& master, std::vector<vk::PhysicalDevice>& devices)
{
	if (devices.size() > 1)
	{
		std::cout << "Select master graphics card, by selecting device ID" << std::endl;
		std::cout << "ID \t Name" << std::endl;

		for (int i = 0; i < devices.size(); i++)
		{
			vk::PhysicalDevice device = devices[i];
			vk::PhysicalDeviceProperties deviceProperties;
			device.getProperties(&deviceProperties);
			std::cout << deviceProperties.deviceName << "(" << i << ")" << std::endl;
		}

		int choice;
		std::cin >> choice;
		master = devices[choice];
		devices.erase(devices.begin() + choice);
	}
	else
	{
		master = devices[0];
		devices.erase(devices.begin());
	}
		

}

std::vector<const char*> Renderer::getRequiredExtensions()
{

	unsigned int extensionCount;

	SDL_Vulkan_GetInstanceExtensions(Window::window, &extensionCount, nullptr);

	std::vector<const char*> extensions(extensionCount);

	SDL_Vulkan_GetInstanceExtensions(Window::window, &extensionCount, extensions.data());

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL Renderer::debugCallback(vk::DebugReportFlagsEXT flags,
	vk::DebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, 
	const char * layerPrefix, const char * msg, void * userData)
{
	std::cerr << "validation layer: " << msg << std::endl;

	return VK_FALSE;
}

vk::Result Renderer::CreateDebugReportCallbackEXT(vk::Instance instance, const vk::DebugReportCallbackCreateInfoEXT * pCreateInfo, const vk::AllocationCallbacks * pAllocator, vk::DebugReportCallbackEXT * pCallback)
{ 

	auto func = (PFN_vkCreateDebugReportCallbackEXT) instance.getProcAddr("vkCreateDebugReportCallbackEXT");
    if (func != nullptr) {
        return (vk::Result) func(instance, (VkDebugReportCallbackCreateInfoEXT*)pCreateInfo, (VkAllocationCallbacks*) (pAllocator), (VkDebugReportCallbackEXT*)pCallback);
    } else {
        return vk::Result::eErrorExtensionNotPresent;
    }
}

void Renderer::DestroyDebugReportCallbackEXT(vk::Instance instance, vk::DebugReportCallbackEXT callback, const vk::AllocationCallbacks * pAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr) {
		func(instance, callback, (VkAllocationCallbacks*) (pAllocator));
	}
}
