#pragma 
#include <pch/pch.h>

namespace core
{
	class VulkanSemaphore
	{
	public:
		VulkanSemaphore() = default;
		~VulkanSemaphore() = default;
		
		void Initialize();

		VkSemaphore GetVulkanSemaphore() const { return m_Semaphore; }

	private:
		VkSemaphore m_Semaphore;
	};
}