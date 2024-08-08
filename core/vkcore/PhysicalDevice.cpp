#include "PhysicalDevice.h"
#include "VulkanUtils.h"

namespace core
{
    PhysicalDevice::PhysicalDevice(VkInstance instance)
        :m_Instance(instance)
    {

    }

    PhysicalDevice::~PhysicalDevice()
    {
    }

    void PhysicalDevice::Create(VkDevice device)
    {
        TORCH_LOG_INFO("Initialize physical device");
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            TORCH_LOG_ERROR("[{}:{}] {}", __FILE__, __LINE__, "Failed to find GPUs with Vulkan Supports.");
            throw std::runtime_error("Failed to find GPUs with Vulkan Support.");
            return;
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        for (const auto& device : devices)
        {
            if (IsDeviceSuitable(device))
            {
                m_PhysicalDevice = device;
                break;
            }
        }

        if (m_PhysicalDevice == VK_NULL_HANDLE)
        {
            TORCH_LOG_ERROR("[{}:{}] {}", __FILE__, __LINE__, "Failed to find a suitable GPU.");
            throw std::runtime_error("Failed to find a suitable GPU.");
        }


    }

    void PhysicalDevice::Destroy(VkDevice device)
    {
        if (device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(device, nullptr);
            device = VK_NULL_HANDLE;
        }
    }

    bool PhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = VulkanUtils::FindQueueFamilies(device);
        return indices.IsComplete();
    }
     
    


}