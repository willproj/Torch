#include "TorchVulkanContext.h"
#include "core/platform/Window.h"

namespace core
{
	TorchVulkanContext::TorchVulkanContext()
	{
		TORCH_LOG_INFO("Initialize the Vulkan.");


		TORCH_LOG_INFO("Finished initialization of Vulkan.");
	}
	TorchVulkanContext::~TorchVulkanContext()
	{
	}
	
	void TorchVulkanContext::CreateInstance()
	{
		TORCH_LOG_INFO("Create vulkan instance.");
		uint32_t availableLayerCount;
		vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
	}
}