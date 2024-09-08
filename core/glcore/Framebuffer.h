#pragma once
#include <pch/pch.h>

namespace core
{
    class Framebuffer
    {
    public:
        Framebuffer();
        ~Framebuffer();

        void Bind() const;
        void Unbind() const;

        void AddAttachments(uint32_t width, uint32_t height, GLint internalFormat, GLenum format, GLenum type);
    private:

        uint32_t m_BufferID;
        std::vector<uint32_t> m_Attachments;
    };
}