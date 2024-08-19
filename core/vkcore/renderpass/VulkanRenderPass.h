#pragma once
#include <pch/pch.h>

namespace core
{
	class VulkanLogicDevice;
	class VulkanSwapChain;

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass() = default;
		~VulkanRenderPass() = default;

		void Initialize(const VulkanLogicDevice& vkLogicDevice, const VulkanSwapChain& vkSwapChain);
		void Destroy(const VulkanLogicDevice& vkLogicDevice);
		
		VkRenderPass GetVulkanRenderPass() const { return m_RenderPass; }
	private:
		VkRenderPass m_RenderPass;
	};
}