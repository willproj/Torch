#pragma once
#include <pch/pch.h>

namespace core
{
	class VulkanPhysicalDevice;
	class VulkanWindowSurface;

	class VulkanLogicDevice
	{
	public:
		VulkanLogicDevice() = default;
		~VulkanLogicDevice() = default;

		VkDevice GetLogicDevice() const { return m_LogicDevice; }
		void Initialize(VulkanPhysicalDevice vkPhysicalDevice, VulkanWindowSurface vkWindowSurface);
		void Destroy();

		VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
		VkQueue GetPresentQueue() const { return m_PresentQueue; }


	private:
		VkDevice m_LogicDevice;
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
	};
}