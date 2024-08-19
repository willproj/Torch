#pragma once
#include <pch/pch.h>

namespace core
{
	class VulkanSwapChain;
	class VulkanLogicDevice;

	class VulkanImageViews
	{
	public:
		VulkanImageViews() = default;
		~VulkanImageViews() = default;
		
		void Initialize(const VulkanSwapChain& vkSwapChain, const VulkanLogicDevice& vkLogicDevice);
		void Destroy(const VulkanLogicDevice& vkLogicDevice);

		std::vector<VkImageView> GetSwapChainImageViews() const { return m_SwapChainImageViews; }
	private:
		std::vector<VkImageView> m_SwapChainImageViews;
	};
}