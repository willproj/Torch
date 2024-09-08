#include "HDR.h"

namespace core
{
	HDR::HDR()
		:m_Gamma(2.2)
	{
        m_WindowPtr = utils::ServiceLocator::GetWindow();
        m_HDRShader = Shader(
            std::string(PROJECT_ROOT) + "/assets/shader/lightingPass.vert",
            std::string(PROJECT_ROOT) + "/assets/shader/hdr.frag");
        Create();
	}
	HDR::~HDR()
	{
        if (m_HdrFBO)
        {
            glDeleteFramebuffers(1, &m_HdrFBO);
        }
        if (m_RboDepth)
        {
            glDeleteRenderbuffers(1, &m_RboDepth);
        }
	}
	void HDR::Create()
	{
        glGenFramebuffers(1, &m_HdrFBO);
        // create floating point color buffer
        glGenTextures(1, &m_ColorBuffer);
        glBindTexture(GL_TEXTURE_2D, m_ColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, m_HdrFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorBuffer, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

    void HDR::OnUpdate()
    {
        if (m_HdrFBO)
        {
            glDeleteFramebuffers(1, &m_HdrFBO);
        }
        if (m_RboDepth)
        {
            glDeleteRenderbuffers(1, &m_RboDepth);
        }

        Create();

    }
    void HDR::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_HdrFBO);
    }
    void HDR::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}