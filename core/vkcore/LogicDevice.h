#pragma once
#include <pch/pch.h>
#include "VulkanObject.h"


namespace core
{
	class LogicDevice :public VulkanObject
	{
	public:
		LogicDevice() = default;
		LogicDevice(VkInstance instance, VkPhysicalDevice physicalDevice);
		~LogicDevice() override;

		void Create(VkDevice device) override;
		void Destroy(VkDevice device) override;
	private:
		VkPhysicalDevice m_PhysicalDevice;
		VkInstance m_Instance;
		VkQueue m_GraphicsQueue;
		VkDevice m_LogicDevice;
	};
}
