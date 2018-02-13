#include "Renderer.h"

Renderer::Renderer()
{
	createInstance();
	setupDebugCallBack();
}

Renderer::~Renderer()
{
	DestroyDebugReportCallbackEXT(instance, callback, nullptr);
	vkDestroyInstance(instance, nullptr);
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

std::vector<const char*> Renderer::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
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
