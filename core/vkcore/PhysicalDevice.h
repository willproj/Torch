#pragma once
#include <pch/pch.h>
#include "VulkanObject.h"

namespace core
{
    class PhysicalDevice : public VulkanObject
    {
    public:
        PhysicalDevice(VkInstance instance);
        PhysicalDevice() = default;
        ~PhysicalDevice() override;

        void Create(VkDevice device) override;
        void Destroy(VkDevice device) override;

        VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
    private:
        bool IsDeviceSuitable(VkPhysicalDevice device);

        VkInstance m_Instance;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    };
}