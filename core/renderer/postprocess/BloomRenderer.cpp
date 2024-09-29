#include "BloomRenderer.h"
#include <utils/ServiceLocator.h>
#include "core/renderer/ShaderManager.h"
#include "core/glcore/Texture.h"
#include "core/renderer/RenderQuad.h"

namespace core
{
	void BloomRenderer::Initialize()
	{
		auto winSpec = utils::ServiceLocator::GetWindow()->GetWinSpecification();
		m_SrcViewportSize = glm::ivec2(winSpec.width, winSpec.height);
		m_SrcViewportSizeFloat = glm::vec2((float)winSpec.width, (float)winSpec.height);

		m_Bloom.Initialize();
        Create(winSpec.width, winSpec.height);
	}

    void BloomRenderer::Create(uint32_t width, uint32_t height)
    {
        RenderDownSample();
        RenderUpSample();
        glViewport(0, 0, width, height);
    }

    void BloomRenderer::RenderDownSample()
    {
        const std::vector<bloomMip>& mipChain = m_Bloom.GetMipChain();
        auto& downSample = ShaderManager::GetInstance()->GetBloomDownSampleShaderRef();
        downSample.use();

        // Iterate over all mip levels
        for (int i = 0; i < (int)mipChain.size(); i++)
        {
            const bloomMip& mip = mipChain[i];
            // Set source resolution and mip level in the shader
            downSample.setInt("mipLevel", i);
            downSample.setBool("useThreshold", i == 0);

            if (i == 0) {
                // For the first pass, use the original HDR source texture
                Texture::BindTexture(0, GL_TEXTURE_2D, m_SrcTexture);
            }
            else {
                // For subsequent passes, use the previous mip level as the source texture
                Texture::BindTexture(0, GL_TEXTURE_2D, mipChain[i - 1].texture);
            }

            // Bind the current mip texture as the destination texture
            // Ensure the format is consistent with the shader's format
            glBindImageTexture(1, mip.texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R11F_G11F_B10F);
            // Dispatch compute shader
            glDispatchCompute(glm::ceil((mip.size.x) / 8.0), glm::ceil((mip.size.y) / 8.0), 1);

            // Ensure all writes are finished before moving on
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }

        // Unbind the shader
        glUseProgram(0);
    }


    void BloomRenderer::RenderUpSample()
    {
        const std::vector<bloomMip>& mipChain = m_Bloom.GetMipChain();
        auto& upSample = ShaderManager::GetInstance()->GetBloomUpSampleShaderRef();
        upSample.use();
        upSample.setFloat("filterRadius", bloomFilterRadius);

        // Iterate through mip levels from highest (5) to lowest (0)
        for (int i = (int)mipChain.size() - 1; i > 0; i--)
        {
            const bloomMip& mip = mipChain[i];
            const bloomMip& nextMip = mipChain[i - 1];

            // Bind the source texture from the current mip level
            Texture::BindTexture(0, GL_TEXTURE_2D, mip.texture);

            // Bind the next mip level texture as the destination texture
            glBindImageTexture(1, nextMip.texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R11F_G11F_B10F);

            // Calculate the number of workgroups, rounding up the size to avoid issues with non-divisible sizes
            GLuint numGroupsX = (GLuint)ceil(m_SrcViewportSize.x / 8.0);
            GLuint numGroupsY = (GLuint)ceil(m_SrcViewportSize.y / 8.0);

            // Dispatch the compute shader
            glDispatchCompute(numGroupsX, numGroupsY, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }

        glUseProgram(0); // Reset the shader program
    }
}