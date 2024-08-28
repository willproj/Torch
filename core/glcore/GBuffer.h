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


        void BindScatteringTexture() const;


        void OnUpdate();
        void Create();

        uint32_t GetRedIntTexture() const {return m_RedInt;}

        uint32_t GetFramebufferID() const { return m_BufferID; }

    private:
        uint32_t m_BufferID;
        uint32_t m_RboDepth;

        std::vector<uint32_t> m_Attachments;
        uint32_t m_GPosition, m_GNormal, m_GColorSpec, m_RedInt, m_GScattering;
    };
}