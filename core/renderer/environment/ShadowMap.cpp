#include "ShadowMap.h"
#include <utils/ServiceLocator.h>

namespace core
{
	ShadowMap::ShadowMap()
	{
        Initialize();
	}

	ShadowMap::~ShadowMap()
	{
	}

	void ShadowMap::Initialize()
	{
		m_ShadowMapWidth = 1024;
		m_ShadowMapHeight = 1024;

        glGenFramebuffers(1, &m_ShadowMapFramebuffer);
        // create depth texture
        glGenTextures(1, &m_ShadowMapTexture);
        glBindTexture(GL_TEXTURE_2D, m_ShadowMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_ShadowMapWidth, m_ShadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowMapFramebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMapTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
    void ShadowMap::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowMapFramebuffer);
    }
    void ShadowMap::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        auto windowSpecifc = utils::ServiceLocator::GetWindow()->GetWinSpecification();
        glViewport(0, 0, windowSpecifc.width, windowSpecifc.height);
    }
}