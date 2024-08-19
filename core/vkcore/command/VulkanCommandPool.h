#pragma once
#include <pch/pch.h>

namespace core
{
	class VulkanPhysicalDevice;
	class VulkanWindowSurface;
	class VulkanLogicDevice;

	class VulkanCommandPool
	{
	public:
		VulkanCommandPool() = default;
		~VulkanCommandPool() = default;

		void Initialize(const VulkanPhysicalDevice& vkPhysicalDevice, const VulkanWindowSurface& vkSurface,const VulkanLogicDevice& vkLogicDevice);
		void Destroy();

		VkCommandPool GetCommandPool() const { return m_CommandPool; }

	private:
		VkCommandPool m_CommandPool;
	};
}