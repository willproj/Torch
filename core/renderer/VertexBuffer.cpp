//#include "VertexBuffer.h"
//#include "vkcore/devices/VulkanLogicDevice.h"
//#include "vkcore/VulkanCommon.h"
//#include "vkcore/devices/VulkanPhysicalDevice.h"
//
//namespace core
//{
//	void VertexBuffer::Initialize(const VulkanPhysicalDevice& vkPhysicalDevice, const VulkanLogicDevice& vkLogicDevice)
//	{
//        VkBufferCreateInfo bufferInfo{};
//        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//        bufferInfo.size = sizeof(vertices[0]) * vertices.size();
//        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
//        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//        if (vkCreateBuffer(vkLogicDevice.GetLogicDevice(), &bufferInfo, nullptr, &m_VertexBuffer) != VK_SUCCESS) {
//            throw std::runtime_error("failed to create vertex buffer!");
//        }
//
//        VkMemoryRequirements memRequirements;
//        vkGetBufferMemoryRequirements(vkLogicDevice.GetLogicDevice(), m_VertexBuffer, &memRequirements);
//
//        VkMemoryAllocateInfo allocInfo{};
//        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//        allocInfo.allocationSize = memRequirements.size;
//        allocInfo.memoryTypeIndex = VulkanCommon::FindMemoryType(vkPhysicalDevice.GetVulkanPhysicalDevice(), memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//
//        if (vkAllocateMemory(vkLogicDevice.GetLogicDevice(), &allocInfo, nullptr, &m_VertexBufferMemory) != VK_SUCCESS) {
//            throw std::runtime_error("failed to allocate vertex buffer memory!");
//        }
//
//        vkBindBufferMemory(vkLogicDevice.GetLogicDevice(), m_VertexBuffer, m_VertexBufferMemory, 0);
//
//        void* data;
//        vkMapMemory(vkLogicDevice.GetLogicDevice(), m_VertexBufferMemory, 0, bufferInfo.size, 0, &data);
//        memcpy(data, vertices.data(), (size_t)bufferInfo.size);
//        vkUnmapMemory(vkLogicDevice.GetLogicDevice(), m_VertexBufferMemory);
//	}
//	void VertexBuffer::Destroy()
//	{
//	}
//}