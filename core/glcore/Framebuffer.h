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

    enum class InternalFormat
    {
        NONE,
        RGB,
        R32I,
        RGBA32F,
        RGBA16F,
        DEPTH_COMPONENT
    };

    enum class Format
    {
        Float,
        Int,
        Unsigned_Byte,
        Depth_Component
    };

    enum class AttachmentType
    {
        ColorAttachment,
        DepthAttachment
    };

    static GLint GetInternalFormat(InternalFormat format)
    {
        switch (format)
        {
        case InternalFormat::NONE:
            return NULL;
            break;
        case InternalFormat::RGB:
            return GL_RGB;
            break;
        case InternalFormat::R32I:
            return GL_R32I;
            break;
        case InternalFormat::RGBA32F:
            return GL_RGBA32F;
            break;
        case InternalFormat::RGBA16F:
            return GL_RGBA16F;
            break;
        case InternalFormat::DEPTH_COMPONENT:
            return GL_DEPTH_COMPONENT;
            break;
        default:
            return 0;
        }
    }

    static GLenum GetFormat(Format format)
    {
        switch (format)
        {
        case Format::Float:
            return GL_FLOAT;
            break;
        case Format::Int:
            return GL_INT;
            break;
        case Format::Unsigned_Byte:
            return GL_UNSIGNED_BYTE;
            break;
        case Format::Depth_Component:
            return GL_DEPTH_COMPONENT;
            break;
        default:
            return 0;
        }
    }

    static GLenum GetAttachmentType(AttachmentType type)
    {
        switch (type)
        {
        case core::AttachmentType::ColorAttachment:
            return GL_COLOR_ATTACHMENT0;
            break;
        case core::AttachmentType::DepthAttachment:
            return GL_DEPTH_ATTACHMENT;
            break;
        default:
            return 0;
        }
    }

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