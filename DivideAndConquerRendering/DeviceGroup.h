#pragma once
#include <set>
#include <memory>
#include <vulkan\vulkan.hpp>
class DeviceContext;
class DeviceGroup {
private:
	std::set<DeviceContext> devices;

public:
	DeviceGroup();
	void addDevice(vk::Instance& instance, vk::PhysicalDevice physicalDevice);
	void addDevice(vk::Instance& instance, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR& surface);
};