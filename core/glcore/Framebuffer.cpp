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

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &m_BufferID);
    }


}