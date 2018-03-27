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
	struct Swapchain {
		vk::Extent2D extent;
		vk::Format imageFormat;
		std::vector<vk::ImageView> imageViews;
		std::vector<vk::Image> images;
		std::vector<vk::Framebuffer> framebuffers;
		std::vector<vk::Framebuffer> finalframebuffers;
		vk::SwapchainKHR swapchain;
		std::vector<vk::CommandBuffer> commandBuffers;
		Texture* depthImage;
	};
	
	struct SecondaryDeviceTexturePair
	{
		RenderTexture* renderTexture;
		Texture* renderDepthTexture;
		Texture* targetTexture;
		Buffer* targetDepthBuffer;
	};

private:
	float loadPercentage;
	Technique* combineTechnique;

	std::map<DeviceContext*, SecondaryDeviceTexturePair*> secondaryDeviceTextures;
	vk::Framebuffer renderTextureFrameBuffer;


	vk::CommandBuffer renderPassCommandBuffer;

	DeviceGroup* deviceGroup;
	DEVICE_MODE mode;

	vk::SurfaceKHR surface;
	vk::RenderPass presentRenderPass;
	
	vk::PhysicalDevice physicalDevice;
	vk::Device device;
	vk::RenderPass renderPass;
	vk::CommandPool commandPool;
	vk::Queue graphicsQueue;
	vk::Queue presentQueue;

	vk::Fence geometryFinished;

	Swapchain swapchain;
	
	vk::Semaphore imageAvailableSemaphore;
	vk::Semaphore renderFinishedSemaphore;

	vk::CommandBuffer transferToCpuCommandBuffer;
	vk::CommandBuffer combineDaQCommandBuffer;

	vk::QueryPool timestamQuery;
	float timestampPeriod;

	uint32_t finalCommandBufferIndex;
	

	struct QueueFamilyIndices
	{
		int graphicsFamily = -1;
		int presentFamily = -1;
		bool headless = false;
		bool isComplete()
		{
			return graphicsFamily >= 0 && (presentFamily >= 0 || headless);
		}
	};

	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	
	std::map<Technique*, std::vector<Mesh*>> renderQueue;
	std::map<Technique*, std::vector<Mesh*>> TransparentRenderQueue;

public:
	DeviceContext(DeviceGroup* group, vk::Instance& instance, vk::PhysicalDevice physicalDevice);
	DeviceContext(DeviceGroup* group, vk::Instance& instance, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR& surface);
	~DeviceContext();

	Swapchain& getSwapchain();

	vk::Device& getDevice();
	vk::Device* getAddressOfDevice();

	vk::RenderPass& getRenderpass(RENDERPASS_TYPE type);

	Texture* getTargetTexture();
	vk::CommandPool getCommandPool();

	void clearBuffer(float r, float g, float b, float a);
	void startFinalRenderPass();
	void tempPresent(bool daqEnabled);
	void executeCommandQueue();

	void submitMesh(Mesh* mesh);
	void renderGeometry();

	void initDevice();

	void setCombineTechnique(Technique* combineTechnique);

	void executeSingleTimeQueue(std::function< void (vk::CommandBuffer)> commands);

	SecondaryDeviceTexturePair* getTexturePair(DeviceContext* deviceContext);
	std::map<DeviceContext*, SecondaryDeviceTexturePair*> getTexturePairs();

	void waitForGeometry();
	void waitForMainDevice();
	void waitForCombinePass();
	float getTimeTaken();

	float getLoadPercentage();
	void setLoadPercentage(float value);

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
private:
	void createDevice(vk::Instance& instance);
	void createRenderPass();
	void createPresentRenderPass();
	void createFrameBuffers();
	void createSecondaryDeviceTexture(DeviceContext* deviceContext);
	void createSecondaryDeviceFramebuffer();
	void createCommandPool();
	void createCommandBuffers();
	void createSemaphores();
	void createQueries();
	
	

	QueueFamilyIndices findQueueFamilies();

	//Swapchain functions
	void createSwapchain();
	void createSwapchainImageViews();
	SwapChainSupportDetails querySwapChainSupport();
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

	DeviceContext* getMainDevice();

};