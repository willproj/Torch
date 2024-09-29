#include "Framebuffer.h"


namespace core
{
    Framebuffer::Framebuffer()
    {
        glGenFramebuffers(1,&m_BufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
    }

    void Framebuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
    }

    void Framebuffer::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::AddAttachments(uint32_t width, uint32_t height, GLint internalFormat, GLenum format, GLenum type)
    {

    }

    void Framebuffer::AddAttachments(const std::initializer_list<Attachment>& attachments)
    {
        m_AttchmentsInfoList.clear(); 
        m_AttchmentsInfoList.insert(m_AttchmentsInfoList.end(), attachments); 
        std::vector<GLenum> glColorAttachments;

        this->Bind();
        int32_t idx = 0;
        for (auto& attachment : attachments)
        {
            glColorAttachments.emplace_back(GL_COLOR_ATTACHMENT0 + idx);
            idx++;
        }

        glDrawBuffers(glColorAttachments.size(), glColorAttachments.data());
        this->CheckFramebuffer();
        this->Unbind();
    }

    void Framebuffer::OnUpdate(uint32_t width, uint32_t height)
    {
        for (auto& attachment : m_AttchmentsInfoList)
        {
            attachment.width = width;
            attachment.height = height;
        }
    }

    void Framebuffer::CheckFramebuffer()
    {
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &m_BufferID);
        m_Attachments.clear();
        m_AttchmentsInfoList.clear();
    }


}