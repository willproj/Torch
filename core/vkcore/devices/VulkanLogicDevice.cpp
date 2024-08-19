#include "VulkanLogicDevice.h"
#include "vkcore/devices/VulkanPhysicalDevice.h"
#include "vkcore/surface/VulkanWindowSurface.h"
#include "vkcore/validationlayer/VulkanValidationLayers.h"
#include "vkcore/VulkanCommon.h"
namespace core
{
	void VulkanLogicDevice::Initialize(VulkanPhysicalDevice vkPhysicalDevice, VulkanWindowSurface vkWindowSurface)
	{
        QueueFamilyIndices indices = VulkanCommon::FindQueueFamilies(vkPhysicalDevice.GetVulkanPhysicalDevice(), vkWindowSurface.GetVulkanSurface());

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(VulkanValidationLayers::deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = VulkanValidationLayers::deviceExtensions.data();

        if (VulkanValidationLayers::enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanValidationLayers::validationLayers.size());
            createInfo.ppEnabledLayerNames = VulkanValidationLayers::validationLayers.data();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(vkPhysicalDevice.GetVulkanPhysicalDevice(), &createInfo, nullptr, &m_LogicDevice) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(m_LogicDevice, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_LogicDevice, indices.presentFamily.value(), 0, &m_PresentQueue);
	}

	void VulkanLogicDevice::Destroy()
	{
        vkDestroyDevice(m_LogicDevice, nullptr);
	}
}