#pragma once
#include <pch/pch.h>

namespace core
{
    class VulkanInstance
    {
    public:
        VulkanInstance();
        ~VulkanInstance();

        void Initialize();

    private:

        VkInstance m_Instance;

    };
}