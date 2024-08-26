#pragma once
#include <pch/pch.h>

namespace core  
{
    class GBuffer
    {
    public:
        GBuffer();
        ~GBuffer();

        void Bind() const;
        void Unbind() const;

        void BindPositionTexture() const;
        void BindNormalTexture() const;
        void BindColorTexture() const;

        void OnUpdate();
        void Create();

    private:
        uint32_t m_BufferID;
        uint32_t m_RboDepth;

        std::vector<uint32_t> m_Attachments;
        uint32_t m_GPosition, m_GNormal, m_GColorSpec;
    };
}