#pragma once
#include "VulkanObject.h"

namespace core
{
	class VulkanUtils
	{
	public:
		static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	};
}