#include "LogicDevice.h"
#include "VulkanUtils.h"

namespace core
{
	LogicDevice::LogicDevice(VkInstance instance, VkPhysicalDevice physicalDevice)
		:m_Instance(instance), m_PhysicalDevice(physicalDevice)
	{

	}

	LogicDevice::~LogicDevice()
	{
	}

	void LogicDevice::Create(VkDevice device)
	{
		QueueFamilyIndices indices = VulkanUtils::FindQueueFamilies(m_PhysicalDevice);

		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;

		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures{};
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = 0;
		createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicDevice) != VK_SUCCESS)
		{
			TORCH_LOG_ERROR("[{}:{}] {}", __FILE__, __LINE__, "Failed to create logic device.");
			throw std::runtime_error("Failed to create logic device.");
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
	}

	void LogicDevice::Destroy(VkDevice device)
	{
	}
}