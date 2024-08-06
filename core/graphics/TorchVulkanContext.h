#pragma once
#include <pch/pch.h>
#include "TorchGraphicsContext.h"

namespace core
{
	class TorchVulkanContext :public TorchGraphicsContext
	{
	public:
		TorchVulkanContext();
		~TorchVulkanContext();
	private:

		void CreateInstance();

	};
}