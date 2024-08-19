#pragma once
#include <pch/pch.h>
#include "core/vkcore/VulkanCommon.h"

namespace core
{
	class VulkanInstance;
	class VulkanWindowSurface;


	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice() = default;
		~VulkanPhysicalDevice() = default;

		void Initialize(const VulkanInstance& vkInstance, const VulkanWindowSurface& surface);
		void Destroy();

		VkPhysicalDevice GetVulkanPhysicalDevice() const { return m_PhysicalDevice; }

	private:
		VkPhysicalDevice m_PhysicalDevice;
	};
}