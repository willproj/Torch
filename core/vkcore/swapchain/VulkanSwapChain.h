#pragma once
#include <pch/pch.h>

namespace core
{
	class VulkanPhysicalDevice;
	class VulkanWindowSurface;
	class VulkanLogicDevice;

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain() = default;
		~VulkanSwapChain() = default;
		void Initialize(const VulkanPhysicalDevice& vkPhysicalDevice, const VulkanWindowSurface& vkSurface, const VulkanLogicDevice& vkLogicDevice);
		void Destroy(const VulkanLogicDevice& vkLogicDevice);
		
		VkSwapchainKHR GetSwapChain() const { return m_SwapChain; }
		std::vector<VkImage> GetSwapChainImages() const { return m_SwapChainImages; }
		VkFormat GetSwapChainImageFormat() const { return m_SwapChainImageFormat; }
		VkExtent2D GetSwapChainExtent() const { return m_SwapChainExtent; }

	private:
		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	};
}