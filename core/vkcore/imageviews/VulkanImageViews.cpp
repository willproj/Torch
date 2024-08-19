#include "vkcore/imageviews/VulkanImageViews.h"
#include "vkcore/swapchain/VulkanSwapChain.h"
#include "vkcore/devices/VulkanLogicDevice.h"

namespace core
{
	void VulkanImageViews::Initialize(const VulkanSwapChain& vkSwapChain, const VulkanLogicDevice& vkLogicDevice)
	{
        m_SwapChainImageViews.resize(vkSwapChain.GetSwapChainImages().size());

        for (size_t i = 0; i < vkSwapChain.GetSwapChainImages().size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = vkSwapChain.GetSwapChainImages()[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = vkSwapChain.GetSwapChainImageFormat();
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(vkLogicDevice.GetLogicDevice(), &createInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
	}

	void VulkanImageViews::Destroy(const VulkanLogicDevice& vkLogicDevice)
	{
        if (vkLogicDevice.GetLogicDevice() != VK_NULL_HANDLE)
        {
            for (auto imageView : m_SwapChainImageViews) {
                if (imageView != VK_NULL_HANDLE) {
                    vkDestroyImageView(vkLogicDevice.GetLogicDevice(), imageView, nullptr);
                }
            }
            m_SwapChainImageViews.clear();
        }
	}
}

