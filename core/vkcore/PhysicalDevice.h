#pragma once
#include <pch/pch.h>

namespace core
{
    class PhysicalDevice
    {
    public:
        PhysicalDevice();
        ~PhysicalDevice();

        void Initialize();
    private:
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    };
}