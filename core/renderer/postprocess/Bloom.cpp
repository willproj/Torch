#include "Bloom.h"
#include <utils/ServiceLocator.h>

namespace core
{
	void Bloom::Initialize()
	{
		Create();
	}

	void Bloom::Create()
	{
		for (auto& mip : m_MipChain) {
			if (mip.texture != 0) {
				glDeleteTextures(1, &mip.texture);
				mip.texture = 0;
			}
		}
		m_MipChain.clear();

		auto winSpec = utils::ServiceLocator::GetWindow()->GetWinSpecification();

		glm::vec2 mipSize((float)winSpec.width, (float)winSpec.height);
		glm::ivec2 mipIntSize((int)winSpec.width, (int)winSpec.height);
		// Safety check
		if (winSpec.width > (unsigned int)INT_MAX || winSpec.height > (unsigned int)INT_MAX) {
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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			//std::cout << "Created bloom mip " << mipIntSize.x << 'x' << mipIntSize.y << std::endl;
			m_MipChain.emplace_back(mip);

		}
		return;
	}

}

