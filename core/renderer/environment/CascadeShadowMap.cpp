#include "CascadeShadowMap.h"
#include <utils/ServiceLocator.h>

namespace core
{
    CascadeShadowMap::CascadeShadowMap(std::shared_ptr<EditorCamera> editorCamera)
	{
        m_EditorCameraPtr = editorCamera;
        Initialize();
	}

    CascadeShadowMap ::~CascadeShadowMap()
	{
	}

	void CascadeShadowMap::Initialize()
	{
        auto& specific = std::get<CascadeShadowMapSpecification>(m_Specification);
        glGenFramebuffers(1, &specific.shadowMapFramebuffer);
        // create depth texture
        glGenTextures(1, &specific.shadowMapTexture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, specific.shadowMapTexture);
        glTexImage3D(
            GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, specific.depthMapResolution, specific.depthMapResolution, int(specific.shadowCascadeLevels.size()) + 1,
            0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

        glBindFramebuffer(GL_FRAMEBUFFER, specific.shadowMapFramebuffer);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, specific.shadowMapTexture, 0);
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
    void CascadeShadowMap::Bind()
    {
        auto& specific = std::get<CascadeShadowMapSpecification>(m_Specification);
        glBindFramebuffer(GL_FRAMEBUFFER, specific.shadowMapFramebuffer);
    }
    void CascadeShadowMap::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_EditorCameraPtr->GetViewportWidth(), m_EditorCameraPtr->GetViewportHeight());
    }

    void CascadeShadowMap::Render()
    {
        
        
    }

    void CascadeShadowMap::BeginRender()
    {
        auto& specific = std::get<CascadeShadowMapSpecification>(m_Specification);
        UBOSetup();
        Bind();
        glViewport(0, 0, specific.depthMapResolution, specific.depthMapResolution);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
    }

    void CascadeShadowMap::EndRender()
    {
        glCullFace(GL_BACK);
        Unbind();
    }

    std::vector<glm::vec4> CascadeShadowMap::GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
    {
        return GetFrustumCornersWorldSpace(proj * view);
    }


    std::vector<glm::vec4> CascadeShadowMap::GetFrustumCornersWorldSpace(const glm::mat4& projview)
    {
        const auto inv = glm::inverse(projview);

        std::vector<glm::vec4> frustumCorners;
        for (unsigned int x = 0; x < 2; ++x)
        {
            for (unsigned int y = 0; y < 2; ++y)
            {
                for (unsigned int z = 0; z < 2; ++z)
                {
                    const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                    frustumCorners.push_back(pt / pt.w);
                }
            }
        }

        return frustumCorners;
    }

    glm::mat4 CascadeShadowMap::GetLightSpaceMatrix(const float nearPlane, const float farPlane)
    {
        const auto proj = glm::perspective(
            glm::radians(m_EditorCameraPtr->GetZoom()), (float)m_EditorCameraPtr->GetViewportWidth() / (float)m_EditorCameraPtr->GetViewportHeight(), nearPlane,
            farPlane);
        const auto corners = GetFrustumCornersWorldSpace(proj, m_EditorCameraPtr->GetViewMatrix());

        glm::vec3 center = glm::vec3(0, 0, 0);
        for (const auto& v : corners)
        {
            center += glm::vec3(v);
        }
        center /= corners.size();
        auto atmosphere = EnvironmentManager::GetInstance()->GetEnvironmentEntityPtr(EnvironmentEntityType::Atmosphere)->GetSpecification();
        const auto lightView = glm::lookAt(center + std::get<AtmosphericScatteringSpecification>(atmosphere.get()).sunPosition, center, glm::vec3(0.0f, 1.0f, 0.0f));

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::lowest();
        for (const auto& v : corners)
        {
            const auto trf = lightView * v;
            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        // Tune this parameter according to the scene
        constexpr float zMult = 10.0f;
        if (minZ < 0)
        {
            minZ *= zMult;
        }
        else
        {
            minZ /= zMult;
        }
        if (maxZ < 0)
        {
            maxZ /= zMult;
        }
        else
        {
            maxZ *= zMult;
        }

        const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
        return lightProjection * lightView;
    }

    std::vector<glm::mat4>  CascadeShadowMap::GetLightSpaceMatrices()
    {
        std::vector<glm::mat4> ret;
        auto& specific = std::get<CascadeShadowMapSpecification>(m_Specification);
        for (size_t i = 0; i < specific.shadowCascadeLevels.size() + 1; ++i)
        {
            if (i == 0)
            {
                ret.push_back(GetLightSpaceMatrix(m_EditorCameraPtr->GetNearClip(), specific.shadowCascadeLevels[i]));
            }
            else if (i < specific.shadowCascadeLevels.size())
            {
                ret.push_back(GetLightSpaceMatrix(specific.shadowCascadeLevels[i - 1], specific.shadowCascadeLevels[i]));
            }
            else
            {
                ret.push_back(GetLightSpaceMatrix(specific.shadowCascadeLevels[i - 1], m_EditorCameraPtr->GetFarClip()));
            }
        }
        return ret;
    }

    void CascadeShadowMap::UBOSetup()
    {
        const auto lightMatrices = GetLightSpaceMatrices();
        glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
        for (size_t i = 0; i < lightMatrices.size(); ++i)
        {
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void CascadeShadowMap::DrawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader)
    {
        visualizerVAOs.resize(8);
        visualizerEBOs.resize(8);
        visualizerVBOs.resize(8);

        const GLuint indices[] = {
            0, 2, 3,
            0, 3, 1,
            4, 6, 2,
            4, 2, 0,
            5, 7, 6,
            5, 6, 4,
            1, 3, 7,
            1, 7, 5,
            6, 7, 3,
            6, 3, 2,
            1, 5, 4,
            0, 1, 4
        };

        const glm::vec4 colors[] = {
            {1.0, 0.0, 0.0, 0.5f},
            {0.0, 1.0, 0.0, 0.5f},
            {0.0, 0.0, 1.0, 0.5f},
        };

        for (int i = 0; i < lightMatrices.size(); ++i)
        {
            const auto corners = GetFrustumCornersWorldSpace(lightMatrices[i]);
            std::vector<glm::vec3> vec3s;
            for (const auto& v : corners)
            {
                vec3s.push_back(glm::vec3(v));
            }

            glGenVertexArrays(1, &visualizerVAOs[i]);
            glGenBuffers(1, &visualizerVBOs[i]);
            glGenBuffers(1, &visualizerEBOs[i]);

            glBindVertexArray(visualizerVAOs[i]);

            glBindBuffer(GL_ARRAY_BUFFER, visualizerVBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, vec3s.size() * sizeof(glm::vec3), &vec3s[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, visualizerEBOs[i]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

            glBindVertexArray(visualizerVAOs[i]);
            shader->setVec4("color", colors[i % 3]);
            glDrawElements(GL_TRIANGLES, GLsizei(36), GL_UNSIGNED_INT, 0);

            glDeleteBuffers(1, &visualizerVBOs[i]);
            glDeleteBuffers(1, &visualizerEBOs[i]);
            glDeleteVertexArrays(1, &visualizerVAOs[i]);

            glBindVertexArray(0);
        }

        visualizerVAOs.clear();
        visualizerEBOs.clear();
        visualizerVBOs.clear();
    }
}