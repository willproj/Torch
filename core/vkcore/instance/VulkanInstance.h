#pragma once
#include <pch/pch.h>

namespace core
{
	class VulkanInstance
	{
	public:
		VulkanInstance();
		~VulkanInstance() = default;

		VkInstance GetVulkanInstance() const { return m_Instance; }
		VkAllocationCallbacks* GetAllocator() { return m_Allocator; }

		void Initialize();
		void Destroy();
	private:
		VkInstance m_Instance;
		VkAllocationCallbacks* m_Allocator = nullptr;
	};
}