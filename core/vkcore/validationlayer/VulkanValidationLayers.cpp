#include "VulkanValidationLayers.h"

namespace core
{
    VkInstance VulkanValidationLayers::m_Instance;
    VkDebugUtilsMessengerEXT VulkanValidationLayers::debugMessenger;
    const std::vector<const char*> VulkanValidationLayers::validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const std::vector<const char*> VulkanValidationLayers::deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    void VulkanValidationLayers::Destroy()
    {
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(m_Instance, debugMessenger, nullptr);
        }
    }
}