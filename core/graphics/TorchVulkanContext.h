#pragma once
#include <pch/pch.h>
#include "TorchGraphicsContext.h"
#include "core/vkcore/instance/VulkanInstance.h"
#include "core/vkcore/validationlayer/VulkanValidationLayers.h"
#include "core/vkcore/devices/VulkanPhysicalDevice.h"
#include "core/vkcore/devices/VulkanLogicDevice.h"
#include "core/vkcore/surface/VulkanWindowSurface.h"
#include "core/vkcore/swapchain/VulkanSwapChain.h"
#include "core/vkcore/imageviews/VulkanImageViews.h"
#include "core/vkcore/pipeline/VulkanGraphicsPipeline.h"
#include "core/vkcore/renderpass/VulkanRenderPass.h"
#include "core/vkcore/framebuffer/VulkanFramebuffer.h"
#include "core/vkcore/command/VulkanCommandBuffer.h"
#include "core/vkcore/command/VulkanCommandPool.h"
#include "core/vkcore/syncobjects/VulkanSyncObjects.h"
//#include "core/renderer/VertexBuffer.h"

namespace core
{
	class TorchVulkanContext : public TorchGraphicsContext
	{
	public:
		TorchVulkanContext();
		~TorchVulkanContext();
		void DrawFrame() override;
		void OnUpdate() override;

		APIType GetAPIType() const override { return APIType::Vulkan; }

		// Getter methods
		const VulkanInstance &GetInstance() const { return m_Instance; }
		const VulkanPhysicalDevice &GetPhysicalDevice() const { return m_PhysicalDevice; }
		const VulkanLogicDevice &GetLogicalDevice() const { return m_LogicDevice; }
		const VulkanWindowSurface &GetSurface() const { return m_Surface; }
		const VulkanSwapChain &GetSwapChain() const { return m_SwapChain; }
		const VulkanImageViews &GetImageViews() const { return m_ImageViews; }
		const VulkanGraphicsPipeline &GetGraphicsPipeline() const { return m_GraphicsPipeline; }
		const VulkanRenderPass &GetRenderPass() const { return m_RenderPass; }
		const VulkanFramebuffer &GetFramebuffer() const { return m_Framebuffer; }
		const VulkanCommandBuffer &GetCommandBuffer() const { return m_CommandBuffer; }
		const VulkanCommandPool &GetCommandPool() const { return m_CommandPool; }
		const VulkanSyncObjects &GetSyncObjects() const { return m_SyncObjects; }

	private:
		void CreateInstance();
		VulkanInstance m_Instance;
		VulkanPhysicalDevice m_PhysicalDevice;
		VulkanLogicDevice m_LogicDevice;
		VulkanWindowSurface m_Surface;
		VulkanSwapChain m_SwapChain;
		VulkanImageViews m_ImageViews;
		VulkanGraphicsPipeline m_GraphicsPipeline;
		VulkanRenderPass m_RenderPass;
		VulkanFramebuffer m_Framebuffer;
		VulkanCommandBuffer m_CommandBuffer;
		VulkanCommandPool m_CommandPool;
		VulkanSyncObjects m_SyncObjects;
		//VertexBuffer m_VertexBuffer;

		uint32_t currentFrame = 0;

	};
}
