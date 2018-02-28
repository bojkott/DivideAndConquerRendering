#include "Renderer.h"
#include "DeviceContext.h"
#include "Texture.h"
#include "materials\DaQCombineMaterial.h"
#include "Technique.h"
#include "Buffer.h"
const std::vector<const char*> Renderer::validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

DeviceGroup Renderer::deviceGroup;

Renderer::Renderer()
{
	createInstance();
#ifdef _DEBUG
	setupDebugCallBack();
#endif
	createSurface();
	setupDeviceGroup();

	daQCombineTechnique = new Technique(deviceGroup.getMainDevice(), Material::addMaterial<DaQCombineMaterial>(), new RenderState());
	deviceGroup.getMainDevice()->setCombineTechnique(daQCombineTechnique);
	std::vector<uint32_t> verts;
	verts.push_back(1);
	verts.push_back(2);
	verts.push_back(3);
	vkGroups::VertexBufferGroup test;
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

	mainDevice->clearBuffer(1, 1, 0, 1);
	//mainDevice->DrawGeometry
	for (auto& slaveDevices : mainDevice->getTexturePairs())
	{
		DeviceContext* device = slaveDevices.first;

		device->clearBuffer(0, 0, 1, 1);
		//device->DrawGeometry
		device->transferRenderTexture();
		device->executeCommandQueue();
		device->getTexturePair(device)->targetTexture->transferTextureTo(*slaveDevices.second->targetTexture);
		device->getTexturePair(device)->targetDepthBuffer->transferBufferTo(*slaveDevices.second->targetDepthBuffer);
	}
	
	//Sync GPUs

	mainDevice->startFinalRenderPass(); //Combine
	mainDevice->tempPresent(); //Final pass
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

	uint32_t deviceCount = 0;
	instance.enumeratePhysicalDevices(&deviceCount, nullptr);

	if (!deviceCount)
	{
		throw std::runtime_error("Failed to find VULKAN supported gfx card");
	}

	std::vector<vk::PhysicalDevice> physicalDevices(deviceCount);
	instance.enumeratePhysicalDevices(&deviceCount, physicalDevices.data());
	for (auto& device = physicalDevices.begin(); device != physicalDevices.end(); ++device)
	{
		// If it's not suitable, remove it
		if (!isDeviceSuitable(*device))
		{
			physicalDevices.erase(device);
		}
	}

	arrangeGroup(physicalDevices);



	deviceGroup.addDevice(instance, physicalDevices[0], surface);
	for (auto& device = physicalDevices.begin() + 1; device != physicalDevices.end(); device++)
		deviceGroup.addDevice(instance, *device);

	deviceGroup.initDevices();
	
}

bool Renderer::isDeviceSuitable(const vk::PhysicalDevice & device)
{
	vk::PhysicalDeviceProperties deviceProperties;
	vk::PhysicalDeviceFeatures deviceFeatures;

	device.getProperties(&deviceProperties);
	device.getFeatures(&deviceFeatures);

	//return deviceProperties.deviceType == vk::PhysicalDeviceType::eCpu;
	return true; // all is welcome, for now
}

void Renderer::arrangeGroup(std::vector<vk::PhysicalDevice>& devices)
{
	if (devices.size() > 1)
	{
		std::cout << "Select graphics card for head, by selecting device ID" << std::endl;
		std::cout << "ID \t Name" << std::endl;

		// Jag kom fan inte på ett bättre sätt att lösa detta på, förlåt
		std::map<uint32_t, vk::PhysicalDevice> options;
		for (const auto& device : devices)
		{
			vk::PhysicalDeviceProperties deviceProperties;
			device.getProperties(&deviceProperties);
			std::cout << deviceProperties.deviceID << "\t " << deviceProperties.deviceName << std::endl;


			options[deviceProperties.deviceID] = device;
		}

		int choice;
		std::cin >> choice;
		devices.insert(devices.begin(), options[choice]);
		devices.erase(std::unique(devices.begin(), devices.end()), devices.end());
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
