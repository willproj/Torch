#pragma once
#include <pch/pch.h>

namespace core
{
	class VulkanLogicDevice;

	class VulkanSyncObjects
	{
	public:
		VulkanSyncObjects() = default;
		~VulkanSyncObjects() = default;

		void Initialize(VulkanLogicDevice vkLogicDevice);

		VkSemaphore GetImageAvailableSemaphore() const { return m_ImageAvailableSemaphore; }
		VkSemaphore GetRenderFinishedSemaphore() const { return m_RenderFinishedSemaphore; }
		VkFence GetInFlightFence() const { return m_InFlightFence; }

		utils::Ref<VkSemaphore> GetImageAvailableSemaphoreRef() { return utils::Ref<VkSemaphore>(m_ImageAvailableSemaphore); }
		utils::Ref<VkSemaphore> GetRenderFinishedSemaphoreRef() { return utils::Ref<VkSemaphore>(m_RenderFinishedSemaphore); }
		utils::Ref<VkFence> GetInFlightFenceRef() { return utils::Ref<VkFence>(m_InFlightFence); }

	private:
		VkSemaphore m_ImageAvailableSemaphore;
		VkSemaphore m_RenderFinishedSemaphore;
		VkFence m_InFlightFence;
	};
}