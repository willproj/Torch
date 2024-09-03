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
        void BindDepthTexture() const;
        void BindRoughnessAOTexture() const;

        void OnUpdate();
        void Create();
        void Initialize();

        uint32_t GetRedIntTexture() const {return m_RedInt;}
        uint32_t GetFramebufferID() const { return m_BufferID; }

        uint32_t GetGPositionTexture() const { return m_GPosition; }
        uint32_t GetGNormalTexture() const { return m_GNormal; }
        uint32_t GetGColorTexture() const { return m_GColorSpec; }
        uint32_t GetGDepthTexture() const { return m_GDepth; }

    private:
        uint32_t m_BufferID;

        std::vector<uint32_t> m_Attachments;
        uint32_t m_GPosition, m_GNormal, m_GColorSpec, m_GSpecRoughAO, m_RedInt, m_GDepth;
    };
}