#pragma once
#include <pch/pch.h>

namespace core
{
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool IsComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

	class VulkanCommon
	{
	public:

        static bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
        static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
        static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
        static bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        static uint32_t FindMemoryType(const VkPhysicalDevice& device, uint32_t typeFilter, const VkMemoryPropertyFlags& properties);


	};
}