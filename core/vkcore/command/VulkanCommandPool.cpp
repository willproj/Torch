#include "VulkanCommandPool.h"
#include "vkcore/devices/VulkanPhysicalDevice.h"
#include "vkcore/surface/VulkanWindowSurface.h"
#include "vkcore/devices/VulkanLogicDevice.h"
#include "vkcore/VulkanCommon.h"

namespace core
{
	void VulkanCommandPool::Initialize(const VulkanPhysicalDevice& vkPhysicalDevice, const VulkanWindowSurface& vkSurface, const VulkanLogicDevice& vkLogicDevice)
	{
		QueueFamilyIndices queueFamilyIndices = VulkanCommon::FindQueueFamilies(vkPhysicalDevice.GetVulkanPhysicalDevice(), vkSurface.GetVulkanSurface());

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(vkLogicDevice.GetLogicDevice(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
	}
	void VulkanCommandPool::Destroy()
	{
	}
}