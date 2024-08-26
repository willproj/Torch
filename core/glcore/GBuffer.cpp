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

        m_Attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, m_Attachments.data());

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

    void GBuffer::OnUpdate()
    {
        if (m_BufferID || m_RboDepth)
        {
            if (m_BufferID)
            {
                glDeleteFramebuffers(1, &m_BufferID);
            }
            if (m_GPosition)
            {
                glDeleteTextures(1, &m_GPosition);
            }
            if (m_GNormal)
            {
                glDeleteTextures(1, &m_GNormal);
            }
            if (m_GColorSpec)
            {
                glDeleteTextures(1, &m_GColorSpec);
            }
            if (m_RboDepth)
            {
                glDeleteRenderbuffers(1, &m_RboDepth);
            }
               
            Create();
        }
    }

    GBuffer::~GBuffer()
    {
        glDeleteFramebuffers(1, &m_BufferID);
    }
}