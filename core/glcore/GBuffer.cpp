#include "GBuffer.h"
#include <utils/ServiceLocator.h>

namespace core
{
    GBuffer::GBuffer()
    {
    }

    void GBuffer::Initialize()
    {
        uint32_t width = utils::ServiceLocator::GetWindow()->GetWinSpecification().width;
        uint32_t height = utils::ServiceLocator::GetWindow()->GetWinSpecification().height;
        Create(width, height);
    }

    void GBuffer::Create(int width, int height)
    {
        glGenFramebuffers(1, &m_BufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

        // Position buffer
        glGenTextures(1, &m_GPosition);
        glBindTexture(GL_TEXTURE_2D, m_GPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_GPosition, 0);


        // Normal color buffer
        glGenTextures(1, &m_GNormal);
        glBindTexture(GL_TEXTURE_2D, m_GNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_GNormal, 0);

        // Color + specular color buffer (albedo + metallic )
        glGenTextures(1, &m_GColorSpec);
        glBindTexture(GL_TEXTURE_2D, m_GColorSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_GColorSpec, 0);
        
        // roughness and ao
        glGenTextures(1, &m_GSpecRoughAO);
        glBindTexture(GL_TEXTURE_2D, m_GSpecRoughAO);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_GSpecRoughAO, 0);

        // Red int texture
        glGenTextures(1, &m_RedInt);
        glBindTexture(GL_TEXTURE_2D, m_RedInt);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_RedInt, 0);

        // Light Space Position texture
        glGenTextures(1, &m_GLightSpacePosition);
        glBindTexture(GL_TEXTURE_2D, m_GLightSpacePosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, m_GLightSpacePosition, 0);

        // Depth texture
        glGenTextures(1, &m_GDepth);
        glBindTexture(GL_TEXTURE_2D, m_GDepth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_GDepth, 0);


        // View Position buffer ( SSAO need view position buffer)
        glGenTextures(1, &m_GViewPosition);
        glBindTexture(GL_TEXTURE_2D, m_GViewPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, m_GViewPosition, 0);
        

        // List of color attachments (no depth)
        m_Attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
        glDrawBuffers(m_Attachments.size(), m_Attachments.data());

        // Check if framebuffer is complete
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

    void GBuffer::OnUpdate(int width, int height)
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
        if (m_GSpecRoughAO)
        {
            glDeleteTextures(1, &m_GSpecRoughAO);
            m_GSpecRoughAO = 0;
        }
        if (m_RedInt)
        {
            glDeleteTextures(1, &m_RedInt);
            m_RedInt = 0;
        }
        if(m_GLightSpacePosition)
        {
            glDeleteTextures(1, &m_GLightSpacePosition);
            m_GLightSpacePosition = 0;
        }
        if (m_GDepth)
        {
            glDeleteTextures(1, &m_GDepth);
            m_GDepth = 0;
        }
        Create(width, height);
    }

    GBuffer::~GBuffer()
    {
        glDeleteFramebuffers(1, &m_BufferID);
        glDeleteTextures(1, &m_GPosition);
        glDeleteTextures(1, &m_GNormal);
        glDeleteTextures(1, &m_GColorSpec);
        glDeleteTextures(1, &m_RedInt);
        glDeleteTextures(1, &m_GDepth);
        glDeleteTextures(1, &m_GSpecRoughAO);
        glDeleteTextures(1, &m_GLightSpacePosition);
        
    }
}