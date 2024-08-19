#include "vkcore/pipeline/VulkanGraphicsPipeline.h"
#include <utils/FileUtils.h>
#include "vkcore/devices/VulkanLogicDevice.h"
#include "vkcore/renderpass/VulkanRenderPass.h"
#include "renderer/Vertex.h"

namespace core
{
    void VulkanGraphicsPipeline::Initialize(VulkanLogicDevice vkLogicDevice, VulkanRenderPass vkRenderPass)
    {
        try
        {
            m_LogicDevice = vkLogicDevice.GetLogicDevice();

            std::filesystem::path currentPath = std::filesystem::current_path();
            std::filesystem::path projectRoot = currentPath.parent_path().parent_path().parent_path().parent_path();

            std::filesystem::path vertexShaderPath = projectRoot / "assets" / "compiled_shaders" / "vert.spv";
            std::filesystem::path fragmentShaderPath = projectRoot / "assets" / "compiled_shaders" / "frag.spv";

            auto vertexShaderCode = utils::FileUtils::ReadFile(vertexShaderPath.string());
            auto fragmentShaderCode = utils::FileUtils::ReadFile(fragmentShaderPath.string());

            VkShaderModule vertShaderModule = CreateShaderModule(vertexShaderCode);
            VkShaderModule fragShaderModule = CreateShaderModule(fragmentShaderCode);

            VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertShaderStageInfo.module = vertShaderModule;
            vertShaderStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
            fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragShaderStageInfo.module = fragShaderModule;
            fragShaderStageInfo.pName = "main";


            VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

            
            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            
            auto bindingDescription = Vertex::GetBindingDescription();
            auto attributeDescriptions = Vertex::FetAttributeDescriptions();

            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
            vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.scissorCount = 1;

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;

            VkPipelineMultisampleStateCreateInfo multisampling{};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;

            VkPipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY;
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f;
            colorBlending.blendConstants[1] = 0.0f;
            colorBlending.blendConstants[2] = 0.0f;
            colorBlending.blendConstants[3] = 0.0f;

            std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };
            VkPipelineDynamicStateCreateInfo dynamicState{};
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
            dynamicState.pDynamicStates = dynamicStates.data();

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 0;
            pipelineLayoutInfo.pushConstantRangeCount = 0;

            if (vkCreatePipelineLayout(m_LogicDevice, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout!");
            }

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = &dynamicState;
            pipelineInfo.layout = m_PipelineLayout;
            pipelineInfo.renderPass = vkRenderPass.GetVulkanRenderPass();
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

            if (vkCreateGraphicsPipelines(m_LogicDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) {
                throw std::runtime_error("failed to create graphics pipeline!");
            }

            vkDestroyShaderModule(m_LogicDevice, fragShaderModule, nullptr);
            vkDestroyShaderModule(m_LogicDevice, vertShaderModule, nullptr);
            TORCH_LOG_INFO("loading shader successfully");
        }
        catch (const std::exception& e)
        {
            TORCH_LOG_ERROR("[{}:{}] Error of loading shader files: {}", __FILE__, __LINE__, e.what());
            throw std::runtime_error(e.what());
        }
    }


	VkShaderModule VulkanGraphicsPipeline::CreateShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(m_LogicDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shader module.");
		}
		return shaderModule;
	}

	void VulkanGraphicsPipeline::Destroy()
	{
        // Ensure that the Vulkan logical device is valid
        if (m_LogicDevice != VK_NULL_HANDLE)
        {
            // Destroy the graphics pipeline
            if (m_GraphicsPipeline != VK_NULL_HANDLE)
            {
                vkDestroyPipeline(m_LogicDevice, m_GraphicsPipeline, nullptr);
                m_GraphicsPipeline = VK_NULL_HANDLE;
            }

            // Destroy the pipeline layout
            if (m_PipelineLayout != VK_NULL_HANDLE)
            {
                vkDestroyPipelineLayout(m_LogicDevice, m_PipelineLayout, nullptr);
                m_PipelineLayout = VK_NULL_HANDLE;
            }
        }
	}
}