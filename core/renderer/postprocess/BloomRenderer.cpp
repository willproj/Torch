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
        Create();
	}

    void BloomRenderer::Create()
    {
        auto winSpec = utils::ServiceLocator::GetWindow()->GetWinSpecification();
        RenderDownSample();
        //RenderUpSample();
        glViewport(0, 0, winSpec.width, winSpec.height);

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
            if (i == 0)
            {
                downSample.setInt("mipLevel", 1);
            }

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
            glDispatchCompute(glm::ceil(mip.size.x / 16.0), glm::ceil(mip.size.y / 16.0), 1);

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

        for (int i = (int)mipChain.size() - 1; i > 0; i--)
        {
            const bloomMip& mip = mipChain[i];
            const bloomMip& nextMip = mipChain[i - 1];
            // Bind viewport and texture from where to read
            Texture::BindTexture(0, GL_TEXTURE_2D, mip.texture);
            Texture::BindTexture(1, GL_TEXTURE_2D, nextMip.texture);
            glBindImageTexture(2, nextMip.texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R11F_G11F_B10F);
            
            glDispatchCompute((GLuint)ceil(m_SrcViewportSize.x / 16.0), (GLuint)ceil(m_SrcViewportSize.y / 16.0), 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }
        glUseProgram(0);
    }



}