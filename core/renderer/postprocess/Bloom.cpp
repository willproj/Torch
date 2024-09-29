#include "Bloom.h"
#include <utils/ServiceLocator.h>

namespace core
{
	void Bloom::Initialize()
	{
		auto winSpec = utils::ServiceLocator::GetWindow()->GetWinSpecification();
		Create(winSpec.width, winSpec.height);
	}

	void Bloom::Create(uint32_t width, uint32_t height)
	{
		for (auto& mip : m_MipChain) {
			if (mip.texture != 0) {
				glDeleteTextures(1, &mip.texture);
				mip.texture = 0;
			}
		}
		m_MipChain.clear();


		glm::vec2 mipSize((float)width, (float)height);
		glm::ivec2 mipIntSize((int)width, (int)height);
		// Safety check
		if (width > (unsigned int)INT_MAX || height > (unsigned int)INT_MAX) {
			std::cerr << "Window size conversion overflow - cannot build bloom FBO!" << std::endl;
			return;
		}
		for (GLuint i = 0; i < m_MipChainLength; i++)
		{
			bloomMip mip;

			mipSize *= 0.5f;
			mipIntSize /= 2;

			mip.size = mipSize;
			mip.intSize = mipIntSize;

			glGenTextures(1, &mip.texture);
			glBindTexture(GL_TEXTURE_2D, mip.texture);
			// we are downscaling an HDR color buffer, so we need a float texture format
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F,
				(int)mipSize.x, (int)mipSize.y,
				0, GL_RGB, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			//std::cout << "Created bloom mip " << mipIntSize.x << 'x' << mipIntSize.y << std::endl;
			m_MipChain.emplace_back(mip);

		}
		return;
	}

}

