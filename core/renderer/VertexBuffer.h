//#pragma once
//#include <pch/pch.h>
//#include "Vertex.h"
//
//namespace core
//{
//	class VulkanPhysicalDevice;
//	class VulkanLogicDevice;
//	class VertexBuffer
//	{
//	public:
//		VertexBuffer() = default;
//		~VertexBuffer() = default;
//
//		void Initialize(const VulkanPhysicalDevice& vkPhysicalDevice, const VulkanLogicDevice& vkLogicDevice);
//		void Destroy();
//
//		VkBuffer GetVertexBuffer() const { return m_VertexBuffer; }
//		const std::vector<Vertex>& GetVertices() const { return vertices; }
//	private:
//
//		const std::vector<Vertex> vertices = {
//			{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
//			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
//			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
//		};
//
//		VkBuffer m_VertexBuffer;
//		VkDeviceMemory m_VertexBufferMemory;
//
//	};
//}