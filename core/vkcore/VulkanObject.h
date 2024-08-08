#pragma once
#include <pch/pch.h>

namespace core
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		bool IsComplete()
		{
			return graphicsFamily.has_value();
		}
	};

	class VulkanObject
	{
	public:
		virtual ~VulkanObject() = default;

		virtual void Create(VkDevice device) = 0;
		virtual void Destroy(VkDevice device) = 0;
	};
}