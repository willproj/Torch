#pragma once
#include <pch/pch.h>

namespace core
{

    struct Attachment {
        uint32_t width;
        uint32_t height;
        GLint internalFormat;
        GLenum format;
        GLenum type;

        // Constructor for easy initialization
        Attachment(uint32_t w, uint32_t h, GLint ifmt, GLenum fmt, GLenum t)
            : width(w), height(h), internalFormat(ifmt), format(fmt), type(t) {}
    };

    class Framebuffer
    {
    public:
        Framebuffer();
        ~Framebuffer();

        void Bind() const;
        void Unbind() const;

        void AddAttachments(uint32_t width, uint32_t height, GLint internalFormat, GLenum format, GLenum type);
        void AddAttachments(const std::initializer_list<Attachment>& attachments);

        void OnUpdate(uint32_t width, uint32_t height);

    private:
        void CheckFramebuffer();
        uint32_t m_BufferID;
        std::vector<uint32_t> m_Attachments;
        std::vector<Attachment> m_AttchmentsInfoList;
    };
}