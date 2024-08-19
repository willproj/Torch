#include "VulkanWindowSurface.h"
#include "vkcore/instance/VulkanInstance.h"
#include <utils/ServiceLocator.h>

namespace core
{
	void VulkanWindowSurface::Initialize(const VulkanInstance& vkInstance)
	{
		if (glfwCreateWindowSurface(vkInstance.GetVulkanInstance(), utils::ServiceLocator::GetWindow()->GetWinSpecification().glfwWindow, nullptr, &m_Surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	void VulkanWindowSurface::Destroy()
	{

	}
}