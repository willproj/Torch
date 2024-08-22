//#include "VulkanCommandBuffer.h"
//#include "VulkanCommandPool.h"
//#include "vkcore/renderpass/VulkanRenderPass.h"
//#include "vkcore/framebuffer/VulkanFramebuffer.h"
//#include "vkcore/swapchain/VulkanSwapChain.h"
//#include "vkcore/devices/VulkanLogicDevice.h"
//#include "vkcore/pipeline/VulkanGraphicsPipeline.h"
//#include "renderer/VertexBuffer.h"
//
//namespace core
//{
//	void VulkanCommandBuffer::Initialize(VulkanLogicDevice vkLogicDevice, VulkanRenderPass vkRenderPass, VulkanFramebuffer vkFramebuffer, VulkanCommandPool vkCommandPool)
//	{
//        m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
//        VkCommandBufferAllocateInfo allocInfo{};
//        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//        allocInfo.commandPool = vkCommandPool.GetCommandPool();
//        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//        allocInfo.commandBufferCount = 1;
//
//        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
//        {
//            if (vkAllocateCommandBuffers(vkLogicDevice.GetLogicDevice(), &allocInfo, &m_CommandBuffers[i]) != VK_SUCCESS) {
//                throw std::runtime_error("failed to allocate command buffers!");
//            }
//        }
//	}
//
//    void VulkanCommandBuffer::RecordCommandBuffer(VertexBuffer vertexBuffer, VkCommandBuffer commandBuffer, VulkanRenderPass vkRenderPass, VulkanSwapChain vkSwapChain, VulkanFramebuffer vkFramebuffer, VulkanGraphicsPipeline vkGraphicsPipeline, uint32_t imageIndex)
//    {
//        VkCommandBufferBeginInfo beginInfo{};
//        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//
//        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
//            throw std::runtime_error("failed to begin recording command buffer!");
//        }
//
//        VkRenderPassBeginInfo renderPassInfo{};
//        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//        renderPassInfo.renderPass = vkRenderPass.GetVulkanRenderPass();
//        renderPassInfo.framebuffer = vkFramebuffer.GetSwapChainFramebuffer()[imageIndex];
//        renderPassInfo.renderArea.offset = { 0, 0 };
//        renderPassInfo.renderArea.extent = vkSwapChain.GetSwapChainExtent();
//
//        VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
//        renderPassInfo.clearValueCount = 1;
//        renderPassInfo.pClearValues = &clearColor;
//
//        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
//
//        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkGraphicsPipeline.GetGraphicsPipeline());
//
//        VkViewport viewport{};
//        viewport.x = 0.0f;
//        viewport.y = 0.0f;
//        viewport.width = (float)vkSwapChain.GetSwapChainExtent().width;
//        viewport.height = (float)vkSwapChain.GetSwapChainExtent().height;
//        viewport.minDepth = 0.0f;
//        viewport.maxDepth = 1.0f;
//        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
//
//        VkRect2D scissor{};
//        scissor.offset = { 0, 0 };
//        scissor.extent = vkSwapChain.GetSwapChainExtent();
//        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
//
//        VkBuffer vertexBuffers[] = { vertexBuffer.GetVertexBuffer()};
//        VkDeviceSize offsets[] = { 0 };
//        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
//
//        vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertexBuffer.GetVertices().size()), 1, 0, 0);
//
//        vkCmdEndRenderPass(commandBuffer);
//
//        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
//            throw std::runtime_error("failed to record command buffer!");
//        }
//    }
//
//	void VulkanCommandBuffer::Destroy()
//	{
//	}
//}