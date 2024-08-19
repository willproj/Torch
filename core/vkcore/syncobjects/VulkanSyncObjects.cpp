#include "VulkanSyncObjects.h"
#include "vkcore/devices/VulkanLogicDevice.h"

namespace core
{
	void VulkanSyncObjects::Initialize(VulkanLogicDevice vkLogicDevice)
	{
        m_ImageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
        m_InFlightFence.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(vkLogicDevice.GetLogicDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore[i]) != VK_SUCCESS ||
                vkCreateSemaphore(vkLogicDevice.GetLogicDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore[i]) != VK_SUCCESS ||
                vkCreateFence(vkLogicDevice.GetLogicDevice(), &fenceInfo, nullptr, &m_InFlightFence[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
	}
}