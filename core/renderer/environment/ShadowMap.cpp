#include "ShadowMap.h"
#include <utils/ServiceLocator.h>

namespace core
{
	ShadowMap::ShadowMap()
	{
	}

	ShadowMap::~ShadowMap()
	{
	}

	void ShadowMap::Initialize(std::shared_ptr<EditorCamera> editorCamera)
	{
        m_EditorCameraPtr = editorCamera;

        glGenFramebuffers(1, &m_ShadowMapFramebuffer);
        // create depth texture
        glGenTextures(1, &m_ShadowMapTexture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_ShadowMapTexture);
        glTexImage3D(
            GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, m_DepthMapResolution, m_DepthMapResolution, int(m_EditorCameraPtr->shadowCascadeLevels.size()) + 1,
            0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

        glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowMapFramebuffer);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_ShadowMapTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
            throw 0;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenBuffers(1, &matricesUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

	}
    void ShadowMap::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowMapFramebuffer);
    }
    void ShadowMap::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        auto windowSpecifc = utils::ServiceLocator::GetWindow()->GetWinSpecification();
        glViewport(0, 0, windowSpecifc.width, windowSpecifc.height);
    }
}