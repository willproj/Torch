#pragma once
#include <pch/pch.h>
namespace core
{
	class VulkanSwapChain;
	class VulkanImageViews;
	class VulkanRenderPass;
	class VulkanLogicDevice;

	class VulkanFramebuffer
	{
	public:
		VulkanFramebuffer() = default;
		~VulkanFramebuffer() = default;

		void Initialize(const VulkanLogicDevice& vkLogicDevice, const VulkanSwapChain& vkSwapChain, const VulkanImageViews& vkImageViews, const VulkanRenderPass& vkRenderPass);
		void Destroy(const VulkanLogicDevice& vkLogicDevice);

		std::vector<VkFramebuffer> GetSwapChainFramebuffer() const { return m_SwapChainFramebuffer; }

	private:
		std::vector<VkFramebuffer> m_SwapChainFramebuffer;
	};
}