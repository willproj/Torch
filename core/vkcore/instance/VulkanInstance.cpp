#include "VulkanInstance.h"
#include "vkcore/validationlayer/VulkanValidationLayers.h"

namespace core
{
    VulkanInstance::VulkanInstance()
    {
    }

    void VulkanInstance::Initialize()
    {
        if (VulkanValidationLayers::enableValidationLayers && !VulkanValidationLayers::checkValidationLayerSupport()) {
            TORCH_LOG_ERROR("[{}:{}] validation layers requested, but not available!", __FILE__, __LINE__);
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Torch";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = VulkanValidationLayers::getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (VulkanValidationLayers::enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanValidationLayers::validationLayers.size());
            createInfo.ppEnabledLayerNames = VulkanValidationLayers::validationLayers.data();

            VulkanValidationLayers::populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, m_Allocator, &m_Instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
        else
        {
            VulkanValidationLayers::Intialize(m_Instance);
        }
    }

    void VulkanInstance::Destroy()
    {
        vkDestroyInstance(m_Instance, nullptr);
        VulkanValidationLayers::Destroy();
    }
}