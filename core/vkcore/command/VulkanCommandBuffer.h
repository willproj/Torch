#pragma once
#include <pch/pch.h>

namespace core
{
	class VulkanCommandPool;
	class VulkanRenderPass;
	class VulkanFramebuffer;
	class VulkanSwapChain;
	class VulkanGraphicsPipeline;
	class VulkanLogicDevice;
	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer() = default;
		~VulkanCommandBuffer() = default;

		void Initialize(VulkanLogicDevice vkLogicDevice,VulkanRenderPass vkRenderPass, VulkanFramebuffer vkFramebuffer, VulkanCommandPool vkCommandPool);
		void Destroy();

		void RecordCommandBuffer(VulkanRenderPass vkRenderPass, VulkanSwapChain vkSwapChain,  VulkanFramebuffer vkFramebuffer, VulkanGraphicsPipeline vkGraphicsPipeline, uint32_t imageIndex);
		VkCommandBuffer GetVulkanCommandBuffer() const { return m_CommandBuffer; }
		utils::Ref<VkCommandBuffer> GetVulkanCommandBufferRef() { return utils::Ref<VkCommandBuffer>(m_CommandBuffer); }

	private:
		VkCommandBuffer m_CommandBuffer;
	};
}