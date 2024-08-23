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

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &m_BufferID);
    }


}