#pragma once
#include <pch/pch.h>
#include "VulkanObject.h"

namespace core
{
    class VulkanInstance : public VulkanObject
    {
    public:
        VulkanInstance();
        ~VulkanInstance() override;

        void Create(VkDevice device) override;
        void Destroy(VkDevice device) override;

        VkInstance getInstance() const { return m_Instance; }
    private:

        VkInstance m_Instance;

    };
}