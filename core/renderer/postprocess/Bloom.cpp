#include "Bloom.h"
#include <utils/ServiceLocator.h>
#include "core/renderer/ShaderManager.h"
#include "core/glcore/Texture.h"

namespace core
{
	Bloom::~Bloom()
	{
		auto& speci = std::get<BloomSpecification>(m_Specification);
		for (auto& mip : speci.m_MipTextureChain) {
			if (mip.texture != 0) {
				glDeleteTextures(1, &mip.texture);
				mip.texture = 0;
			}
		}
		speci.m_MipTextureChain.clear();
	}

	void Bloom::BeginRender()
	{
		RenderDownSample();
		RenderUpSample();
	}

	void Bloom::EndRender()
	{
	}

	Bloom::Bloom()
	{
		auto& speci = std::get<BloomSpecification>(m_Specification);
		auto winSpec = utils::ServiceLocator::GetWindow()->GetWinSpecification();
		speci.m_SrcViewportSize = glm::ivec2(winSpec.width, winSpec.height);
		speci.m_SrcViewportSizeFloat = glm::vec2((float)winSpec.width, (float)winSpec.height);

		this->Initialize(winSpec.width, winSpec.height);
	}

	void Bloom::Initialize(uint32_t width, uint32_t height)
	{
		auto& speci = std::get<BloomSpecification>(m_Specification);
		speci.m_SrcViewportSizeFloat = glm::ivec2((float)width, (float)height);
		speci.m_SrcViewportSize = glm::vec2((int)width, (int)height);

		// Safety check
		if (width > (unsigned int)INT_MAX || height > (unsigned int)INT_MAX) {
			std::cerr << "Window size conversion overflow - cannot build bloom FBO!" << std::endl;
			return;
		}
		for (GLuint i = 0; i < speci.m_MipChainLength; i++)
		{
			BloomMipTexture mip;

			speci.m_SrcViewportSizeFloat *= 0.5f;
			speci.m_SrcViewportSize /= 2;

			mip.size = speci.m_SrcViewportSizeFloat;
			mip.intSize = speci.m_SrcViewportSize;

			glGenTextures(1, &mip.texture);
			glBindTexture(GL_TEXTURE_2D, mip.texture);
			// we are downscaling an HDR color buffer, so we need a float texture format
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F,
				(int)speci.m_SrcViewportSizeFloat.x, (int)speci.m_SrcViewportSizeFloat.y,
				0, GL_RGB, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			std::cout << "Created bloom mip " << mip.intSize.x << 'x' << mip.intSize.y << std::endl;
			speci.m_MipTextureChain.emplace_back(mip);

		}
	}

	void Bloom::OnUpdate(uint32_t width, uint32_t height)
	{
		auto& speci = std::get<BloomSpecification>(m_Specification);
		for (auto& mip : speci.m_MipTextureChain) {
			if (mip.texture != 0) {
				glDeleteTextures(1, &mip.texture);
				mip.texture = 0;
			}
		}
		speci.m_MipTextureChain.clear();
		this->Initialize(width, height);
	}
	void Bloom::RenderDownSample()
	{
		auto& speci = std::get<BloomSpecification>(m_Specification);
		auto& downSample = ShaderManager::GetInstance()->GetBloomDownSampleShaderRef();
		downSample.use();

		// Iterate over all mip levels
		for (int i = 0; i < (int)speci.m_MipTextureChain.size(); i++)
		{
			const BloomMipTexture& mip = speci.m_MipTextureChain[i];
			// Set source resolution and mip level in the shader
			downSample.setInt("mipLevel", i);
			downSample.setBool("useThreshold", i == 0);

			if (i == 0) {
				// For the first pass, use the original HDR source texture
				Texture::BindTexture(0, GL_TEXTURE_2D, speci.m_SrcTexture);
			}
			else {
				// For subsequent passes, use the previous mip level as the source texture
				Texture::BindTexture(0, GL_TEXTURE_2D, speci.m_MipTextureChain[i - 1].texture);
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

	void Bloom::RenderUpSample()
	{
		auto& speci = std::get<BloomSpecification>(m_Specification);
		auto& upSample = ShaderManager::GetInstance()->GetBloomUpSampleShaderRef();
		upSample.use();
		upSample.setFloat("filterRadius", speci.bloomFilterRadius);

		// Iterate through mip levels from highest (5) to lowest (0)
		for (int i = (int)speci.m_MipTextureChain.size() - 1; i > 0; i--)
		{
			const BloomMipTexture& mip = speci.m_MipTextureChain[i];
			const BloomMipTexture& nextMip = speci.m_MipTextureChain[i - 1];

			// Bind the source texture from the current mip level
			Texture::BindTexture(0, GL_TEXTURE_2D, mip.texture);

			// Bind the next mip level texture as the destination texture
			glBindImageTexture(1, nextMip.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R11F_G11F_B10F);

			// Calculate the number of workgroups, rounding up the size to avoid issues with non-divisible sizes
			GLuint numGroupsX = (GLuint)ceil(nextMip.size.x / 8.0);
			GLuint numGroupsY = (GLuint)ceil(nextMip.size.y / 8.0);

			// Dispatch the compute shader
			glDispatchCompute(numGroupsX, numGroupsY, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}

		glUseProgram(0); // Reset the shader program
	}
}