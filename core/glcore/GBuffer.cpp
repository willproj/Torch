#include "GBuffer.h"
#include <utils/ServiceLocator.h>

namespace core
{
    GBuffer::GBuffer()
    {
        Create();
    }

    void GBuffer::Create()
    {
        glGenFramebuffers(1, &m_BufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

        uint32_t width = utils::ServiceLocator::GetWindow()->GetWinSpecification().width;
        uint32_t height = utils::ServiceLocator::GetWindow()->GetWinSpecification().height;

        // position color buffer
        glGenTextures(1, &m_GPosition);
        glBindTexture(GL_TEXTURE_2D, m_GPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_GPosition, 0);

        // - normal color buffer
        glGenTextures(1, &m_GNormal);
        glBindTexture(GL_TEXTURE_2D, m_GNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_GNormal, 0);

        // - color + specular color buffer
        glGenTextures(1, &m_GColorSpec);
        glBindTexture(GL_TEXTURE_2D, m_GColorSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_GColorSpec, 0);

        // red int texture
        glGenTextures(1, &m_RedInt);
        glBindTexture(GL_TEXTURE_2D, m_RedInt);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_RedInt, 0);

        glGenTextures(1, &m_GScattering);
        glBindTexture(GL_TEXTURE_2D, m_GScattering);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_GScattering, 0);

        m_Attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
        glDrawBuffers(5, m_Attachments.data());

        glGenRenderbuffers(1, &m_RboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, m_RboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RboDepth);
        // finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GBuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
    }

    void GBuffer::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GBuffer::BindPositionTexture() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_GPosition);
    }

    void GBuffer::BindNormalTexture() const
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_GNormal);
    }

    void GBuffer::BindColorTexture() const
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_GColorSpec);
    }

    void GBuffer::BindScatteringTexture() const
    {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, m_GScattering);
    }

    void GBuffer::OnUpdate()
    {
        if (m_BufferID)
        {
            glDeleteFramebuffers(1, &m_BufferID);
            m_BufferID = 0;
        }
        if (m_GPosition)
        {
            glDeleteTextures(1, &m_GPosition);
            m_GPosition = 0;
        }
        if (m_GNormal)
        {
            glDeleteTextures(1, &m_GNormal);
            m_GNormal = 0;
        }
        if (m_GColorSpec)
        {
            glDeleteTextures(1, &m_GColorSpec);
            m_GColorSpec = 0;
        }
        if (m_RedInt)
        {
            glDeleteTextures(1, &m_RedInt);
            m_RedInt = 0;
        }
        if (m_GScattering)
        {
            glDeleteTextures(1, &m_GScattering);
            m_GScattering = 0;
        }
        if (m_RboDepth)
        {
            glDeleteRenderbuffers(1, &m_RboDepth);
            m_RboDepth = 0;
        }

        Create();
    }

    GBuffer::~GBuffer()
    {
        glDeleteFramebuffers(1, &m_BufferID);
    }
}