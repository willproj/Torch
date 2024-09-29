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

        void OnUpdate(int width, int height);
        void Create(int width, int height);
        void Initialize();

        uint32_t GetRedIntTexture() const {return m_RedInt;}
        uint32_t GetFramebufferID() const { return m_BufferID; }

        uint32_t GetGPositionTexture() const { return m_GPosition; }
        uint32_t GetGNormalTexture() const { return m_GNormal; }
        uint32_t GetGColorTexture() const { return m_GColorSpec; }
        uint32_t GetGDepthTexture() const { return m_GDepth; }
        uint32_t GetGLightSpacePosition() const { return m_GLightSpacePosition; }
        uint32_t GetGRoughnessAOTexture() const { return m_GSpecRoughAO; }

        uint32_t GetGViewPositionTexture() const { return m_GViewPosition; }

    private:
        uint32_t m_BufferID;

        std::vector<uint32_t> m_Attachments;
        uint32_t m_GPosition = 0, m_GNormal = 0, m_GColorSpec = 0, m_GSpecRoughAO = 0, m_RedInt = 0, m_GDepth = 0, m_GLightSpacePosition = 0;
        uint32_t m_GViewPosition = 0;
    };
}