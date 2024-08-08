#pragma once
#include <pch/pch.h>
#include "TorchGraphicsContext.h"
#include "core/vkcore/VulkanInstance.h"

namespace core
{
	class TorchVulkanContext :public TorchGraphicsContext
	{
	public:
		TorchVulkanContext();
		~TorchVulkanContext();
	private:

		void CreateInstance();
		VulkanInstance m_Instance;

	};
}