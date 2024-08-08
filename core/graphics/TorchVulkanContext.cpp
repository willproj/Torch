#include "TorchVulkanContext.h"
#include "core/platform/Window.h"

namespace core
{
	TorchVulkanContext::TorchVulkanContext()
	{
		TORCH_LOG_INFO("Initialize the Vulkan.");
		CreateInstance();
		TORCH_LOG_INFO("Finished initialization of Vulkan.");
	}
	
	TorchVulkanContext::~TorchVulkanContext()
	{
	}
	
	void TorchVulkanContext::CreateInstance()
	{
		m_Instance.Initialize();
	}
}