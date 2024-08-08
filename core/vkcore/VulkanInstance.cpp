#include "VulkanInstance.h"

namespace core
{
    VulkanInstance::VulkanInstance()
        :m_Instance(VK_NULL_HANDLE)
    {
    }

    VulkanInstance::~VulkanInstance()
    {
        Destroy(VK_NULL_HANDLE);
    }

    void VulkanInstance::Create(VkDevice device)
    {
        TORCH_LOG_INFO("Create vulkan instance.");
        TORCH_LOG_INFO("Create vk application info.");
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Torch Engine!";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        createInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
        {
            TORCH_LOG_ERROR("[{}:{}] {}", __FILE__, __LINE__, "failed to create instance.");
            throw std::runtime_error("Failed to create instance");
        }
    }
    
    void VulkanInstance::Destroy(VkDevice device)
    {
        if (m_Instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(m_Instance, nullptr);
            m_Instance = VK_NULL_HANDLE;
        }
    }
    

}