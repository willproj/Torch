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
    private:

        uint32_t m_BufferID;
    };
}