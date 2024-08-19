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

		std::vector<VkSemaphore> GetImageAvailableSemaphore() const { return m_ImageAvailableSemaphore; }
		std::vector<VkSemaphore> GetRenderFinishedSemaphore() const { return m_RenderFinishedSemaphore; }
		std::vector<VkFence> GetInFlightFence() const { return m_InFlightFence; }

		utils::Ref<std::vector<VkSemaphore>> GetImageAvailableSemaphoreRef() { return utils::Ref<std::vector<VkSemaphore>>(m_ImageAvailableSemaphore); }
		utils::Ref<std::vector<VkSemaphore>> GetRenderFinishedSemaphoreRef() { return utils::Ref<std::vector<VkSemaphore>>(m_RenderFinishedSemaphore); }
		utils::Ref<std::vector<VkFence>> GetInFlightFenceRef() { return utils::Ref<std::vector<VkFence>>(m_InFlightFence); }

	private:
		std::vector<VkSemaphore> m_ImageAvailableSemaphore;
		std::vector<VkSemaphore> m_RenderFinishedSemaphore;
		std::vector<VkFence> m_InFlightFence;

	};
}