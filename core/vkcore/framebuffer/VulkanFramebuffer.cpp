#include "vkcore/framebuffer/VulkanFramebuffer.h"
#include "vkcore/swapchain/VulkanSwapChain.h"
#include "vkcore/imageviews/VulkanImageViews.h"
#include "vkcore/renderpass/VulkanRenderPass.h"
#include "vkcore/devices/VulkanLogicDevice.h"


namespace core
{
	void VulkanFramebuffer::Initialize(const VulkanLogicDevice& vkLogicDevice, const VulkanSwapChain& vkSwapChain, const VulkanImageViews& vkImageViews, const VulkanRenderPass& vkRenderPass)
	{
        m_SwapChainFramebuffer.resize(vkSwapChain.GetSwapChainImages().size());

        for (size_t i = 0; i < vkSwapChain.GetSwapChainImages().size(); i++) {
            VkImageView attachments[] = {
                vkImageViews.GetSwapChainImageViews()[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = vkRenderPass.GetVulkanRenderPass();
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = vkSwapChain.GetSwapChainExtent().width;
            framebufferInfo.height = vkSwapChain.GetSwapChainExtent().height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(vkLogicDevice.GetLogicDevice(), &framebufferInfo, nullptr, &m_SwapChainFramebuffer[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
	}

	void VulkanFramebuffer::Destroy(const VulkanLogicDevice& vkLogicDevice)
	{
        if (!m_SwapChainFramebuffer.empty())
        {
            for (VkFramebuffer framebuffer : m_SwapChainFramebuffer)
            {
                vkDestroyFramebuffer(vkLogicDevice.GetLogicDevice(), framebuffer, nullptr);
            }
            m_SwapChainFramebuffer.clear();
        }
	}
}