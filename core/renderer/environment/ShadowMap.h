#pragma once
#include <pch/pch.h>
#include "core/renderer/camera/EditorCamera.h"
#include "core/renderer/EnvironmentManager.h"

namespace core
{
	class ShadowMap
	{
	public:
		ShadowMap();
		~ShadowMap();
		void Initialize(std::shared_ptr<EditorCamera> editorCamera);

		void Bind();
		void Unbind();

		uint32_t GetShadowMapTexture() const { return m_ShadowMapTexture; }


        std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview)
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


        std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
        {
            return getFrustumCornersWorldSpace(proj * view);
        }

        glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane)
        {
            const auto proj = glm::perspective(
                glm::radians(m_EditorCameraPtr->GetZoom()), (float)m_EditorCameraPtr->GetViewportWidth() / (float)m_EditorCameraPtr->GetViewportHeight(), nearPlane,
                farPlane);
            const auto corners = getFrustumCornersWorldSpace(proj, m_EditorCameraPtr->GetViewMatrix());

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
        std::vector<glm::mat4> getLightSpaceMatrices()
        {
            std::vector<glm::mat4> ret;
            for (size_t i = 0; i < m_EditorCameraPtr->shadowCascadeLevels.size() + 1; ++i)
            {
                if (i == 0)
                {
                    ret.push_back(getLightSpaceMatrix(m_EditorCameraPtr->GetNearClip(), m_EditorCameraPtr->shadowCascadeLevels[i]));
                }
                else if (i < m_EditorCameraPtr->shadowCascadeLevels.size())
                {
                    ret.push_back(getLightSpaceMatrix(m_EditorCameraPtr->shadowCascadeLevels[i - 1], m_EditorCameraPtr->shadowCascadeLevels[i]));
                }
                else
                {
                    ret.push_back(getLightSpaceMatrix(m_EditorCameraPtr->shadowCascadeLevels[i - 1], m_EditorCameraPtr->GetFarClip()));
                }
            }
            return ret;
        }

        void UBOSetup()
        {
            const auto lightMatrices = getLightSpaceMatrices();
            glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
            for (size_t i = 0; i < lightMatrices.size(); ++i)
            {
                glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
            }
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
        std::vector<glm::mat4> lightMatricesCache;
        uint32_t GetDepthMapResolution() const { return m_DepthMapResolution; }

        unsigned int matricesUBO;





        std::vector<GLuint> visualizerVAOs;
        std::vector<GLuint> visualizerVBOs;
        std::vector<GLuint> visualizerEBOs;

        void drawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader)
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
                const auto corners = getFrustumCornersWorldSpace(lightMatrices[i]);
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


	private:
		uint32_t m_ShadowMapFramebuffer;
        uint32_t m_DepthMapResolution = 4096;
		uint32_t m_ShadowMapTexture;

        std::shared_ptr<EditorCamera> m_EditorCameraPtr;

	};
}