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
	class VertexBuffer;
	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer() = default;
		~VulkanCommandBuffer() = default;

		void Initialize(VulkanLogicDevice vkLogicDevice,VulkanRenderPass vkRenderPass, VulkanFramebuffer vkFramebuffer, VulkanCommandPool vkCommandPool);
		void Destroy();

		void RecordCommandBuffer(VertexBuffer vertexBuffer, VkCommandBuffer commandBuffer, VulkanRenderPass vkRenderPass, VulkanSwapChain vkSwapChain,  VulkanFramebuffer vkFramebuffer, VulkanGraphicsPipeline vkGraphicsPipeline, uint32_t imageIndex);
		std::vector<VkCommandBuffer> GetVulkanCommandBuffer() const { return m_CommandBuffers; }
		utils::Ref<std::vector<VkCommandBuffer>> GetVulkanCommandBufferRef() { return utils::Ref<std::vector<VkCommandBuffer>>(m_CommandBuffers); }

	private:
		std::vector<VkCommandBuffer> m_CommandBuffers;
	};
}