#include "VulkanPhysicalDevice.h"
#include "vkcore/instance/VulkanInstance.h"
#include "vkcore/surface/VulkanWindowSurface.h"
#include "vkcore/validationlayer/VulkanValidationLayers.h"
#include "vkcore/VulkanCommon.h"

namespace core
{
	void VulkanPhysicalDevice::Destroy()
	{

	}

   


	void VulkanPhysicalDevice::Initialize(const VulkanInstance& vkInstance, const VulkanWindowSurface& surface)
	{
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vkInstance.GetVulkanInstance(), &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vkInstance.GetVulkanInstance(), &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (VulkanCommon::IsDeviceSuitable(device, surface.GetVulkanSurface())) {
                m_PhysicalDevice = device;
                break;
            }
        }

        if (m_PhysicalDevice== VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
	}
	
}