#pragma once
#include <pch/pch.h>

namespace core
{
	class VulkanInstance;
	class VulkanWindowSurface
	{
	public:
		VulkanWindowSurface() = default;
		~VulkanWindowSurface() = default;
		
		void Initialize(const VulkanInstance& vkInstance);
		void Destroy();

		VkSurfaceKHR GetVulkanSurface() const { return m_Surface; }
	private:
		VkSurfaceKHR m_Surface;
	};
}