#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "vkcore/renderpass/VulkanRenderPass.h"
#include "vkcore/framebuffer/VulkanFramebuffer.h"
#include "vkcore/swapchain/VulkanSwapChain.h"
#include "vkcore/devices/VulkanLogicDevice.h"
#include "vkcore/pipeline/VulkanGraphicsPipeline.h"

namespace core
{
	void VulkanCommandBuffer::Initialize(VulkanLogicDevice vkLogicDevice, VulkanRenderPass vkRenderPass, VulkanFramebuffer vkFramebuffer, VulkanCommandPool vkCommandPool)
	{
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = vkCommandPool.GetCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(vkLogicDevice.GetLogicDevice(), &allocInfo, &m_CommandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
	}

    void VulkanCommandBuffer::RecordCommandBuffer(VulkanRenderPass vkRenderPass, VulkanSwapChain vkSwapChain, VulkanFramebuffer vkFramebuffer, VulkanGraphicsPipeline vkGraphicsPipeline, uint32_t imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vkRenderPass.GetVulkanRenderPass();
        renderPassInfo.framebuffer = vkFramebuffer.GetSwapChainFramebuffer()[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = vkSwapChain.GetSwapChainExtent();

        VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkGraphicsPipeline.GetGraphicsPipeline());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)vkSwapChain.GetSwapChainExtent().width;
        viewport.height = (float)vkSwapChain.GetSwapChainExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = vkSwapChain.GetSwapChainExtent();
        vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);

        vkCmdDraw(m_CommandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(m_CommandBuffer);

        if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

	void VulkanCommandBuffer::Destroy()
	{
	}
}