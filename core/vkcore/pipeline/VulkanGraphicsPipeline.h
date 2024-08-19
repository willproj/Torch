#pragma once
#include <pch/pch.h>

namespace core
{
	class VulkanLogicDevice;
	class VulkanRenderPass;

	class VulkanGraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline() = default;
		~VulkanGraphicsPipeline() = default;

		void Initialize(VulkanLogicDevice vkLogicDevice, VulkanRenderPass vkRenderPass);
		void Destroy();

		VkPipeline GetGraphicsPipeline() const { return m_GraphicsPipeline; }

	private:
		VkShaderModule CreateShaderModule(const std::vector<char>& code);
		VkDevice m_LogicDevice;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;
	};
}