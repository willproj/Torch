//#include "TorchVulkanContext.h"
//#include "core/platform/Window.h"
//#include <imgui.h>
//#include <backends/imgui_impl_vulkan.h>
//#include <backends/imgui_impl_glfw.h>
//#include <utils/ServiceLocator.h>
//
//namespace core
//{
//	TorchVulkanContext::TorchVulkanContext()
//	{
//		TORCH_LOG_INFO("Initialize the Vulkan.");
//		m_Instance.Initialize();
//		VulkanValidationLayers::setupDebugMessenger();
//		m_Surface.Initialize(m_Instance);
//		m_PhysicalDevice.Initialize(m_Instance, m_Surface);
//		m_LogicDevice.Initialize(m_PhysicalDevice, m_Surface);
//		m_SwapChain.Initialize(m_PhysicalDevice, m_Surface, m_LogicDevice);
//		m_ImageViews.Initialize(m_SwapChain, m_LogicDevice);
//		m_RenderPass.Initialize(m_LogicDevice, m_SwapChain);
//		m_GraphicsPipeline.Initialize(m_LogicDevice, m_RenderPass);
//		m_Framebuffer.Initialize(m_LogicDevice, m_SwapChain, m_ImageViews, m_RenderPass);
//		m_CommandPool.Initialize(m_PhysicalDevice, m_Surface, m_LogicDevice);
//		m_VertexBuffer.Initialize(m_PhysicalDevice, m_LogicDevice);
//		m_CommandBuffer.Initialize(m_LogicDevice, m_RenderPass, m_Framebuffer, m_CommandPool);
//		m_SyncObjects.Initialize(m_LogicDevice);
//		TORCH_LOG_INFO("Finished initialization of Vulkan.");
//	}
//	void TorchVulkanContext::ReCreate()
//	{
//
//		vkDeviceWaitIdle(m_LogicDevice.GetLogicDevice());
//
//		m_Framebuffer.Destroy(m_LogicDevice);
//		m_GraphicsPipeline.Destroy();
//		m_RenderPass.Destroy(m_LogicDevice);
//		m_ImageViews.Destroy(m_LogicDevice);
//		m_SwapChain.Destroy(m_LogicDevice);
//
//		m_SwapChain.Initialize(m_PhysicalDevice, m_Surface, m_LogicDevice);
//		m_ImageViews.Initialize(m_SwapChain, m_LogicDevice);
//		m_RenderPass.Initialize(m_LogicDevice, m_SwapChain);
//		m_GraphicsPipeline.Initialize(m_LogicDevice, m_RenderPass);
//		m_Framebuffer.Initialize(m_LogicDevice, m_SwapChain, m_ImageViews, m_RenderPass);
//		m_CommandPool.Initialize(m_PhysicalDevice, m_Surface, m_LogicDevice);
//		m_CommandBuffer.Initialize(m_LogicDevice, m_RenderPass, m_Framebuffer, m_CommandPool);
//		m_SyncObjects.Initialize(m_LogicDevice);
//	}
//
//	void TorchVulkanContext::DrawFrame()
//	{
//		vkWaitForFences(m_LogicDevice.GetLogicDevice(), 1, &m_SyncObjects.GetInFlightFenceRef().get()[currentFrame], VK_TRUE, UINT64_MAX);
//		vkResetFences(m_LogicDevice.GetLogicDevice(), 1, &m_SyncObjects.GetInFlightFenceRef().get()[currentFrame]);
//
//		uint32_t imageIndex;
//		VkResult result = vkAcquireNextImageKHR(m_LogicDevice.GetLogicDevice(), m_SwapChain.GetSwapChain(), UINT64_MAX, m_SyncObjects.GetImageAvailableSemaphore()[currentFrame], VK_NULL_HANDLE, &imageIndex);
//		if (result != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to acquire swap chain image!");
//		}
//
//		vkResetCommandBuffer(m_CommandBuffer.GetVulkanCommandBuffer()[currentFrame], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
//		m_CommandBuffer.RecordCommandBuffer(m_VertexBuffer, m_CommandBuffer.GetVulkanCommandBuffer()[currentFrame], m_RenderPass, m_SwapChain, m_Framebuffer, m_GraphicsPipeline, imageIndex);
//
//		VkSubmitInfo submitInfo{};
//		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//
//		VkSemaphore waitSemaphores[] = {m_SyncObjects.GetImageAvailableSemaphore()[currentFrame]};
//		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
//		submitInfo.waitSemaphoreCount = 1;
//		submitInfo.pWaitSemaphores = waitSemaphores;
//		submitInfo.pWaitDstStageMask = waitStages;
//
//		submitInfo.commandBufferCount = 1;
//		submitInfo.pCommandBuffers = &m_CommandBuffer.GetVulkanCommandBufferRef().get()[currentFrame];
//
//		VkSemaphore signalSemaphores[] = {m_SyncObjects.GetRenderFinishedSemaphore()[currentFrame]};
//		submitInfo.signalSemaphoreCount = 1;
//		submitInfo.pSignalSemaphores = signalSemaphores;
//
//		if (vkQueueSubmit(m_LogicDevice.GetGraphicsQueue(), 1, &submitInfo, m_SyncObjects.GetInFlightFence()[currentFrame]) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to submit draw command buffer!");
//		}
//
//		VkPresentInfoKHR presentInfo{};
//		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//
//		presentInfo.waitSemaphoreCount = 1;
//		presentInfo.pWaitSemaphores = signalSemaphores;
//
//		VkSwapchainKHR swapChains[] = {m_SwapChain.GetSwapChain()};
//		presentInfo.swapchainCount = 1;
//		presentInfo.pSwapchains = swapChains;
//
//		presentInfo.pImageIndices = &imageIndex;
//
//		result = vkQueuePresentKHR(m_LogicDevice.GetPresentQueue(), &presentInfo);
//		if (result != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to present swap chain image!");
//		}
//	}
//
//	TorchVulkanContext::~TorchVulkanContext()
//	{
//	}
//
//	void TorchVulkanContext::CreateInstance()
//	{
//	}
//}