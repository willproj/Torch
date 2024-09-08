#pragma once
#include "EnvironmentEntity.h"
#include <pch/pch.h>
#include "core/renderer/camera/EditorCamera.h"
#include "core/renderer/EnvironmentManager.h"

namespace core
{
    class CascadeShadowMap : public EnvironmentEntity
	{
	public:
        CascadeShadowMap() = default;
        CascadeShadowMap(std::shared_ptr<EditorCamera> editorCamera);
		~CascadeShadowMap();
		void Initialize() override;

		void Bind();
		void Unbind();

        void Render() override;
        void BeginRender() override;
        void EndRender() override;


        std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& projview);
        std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
        std::vector<glm::mat4> GetLightSpaceMatrices();
        glm::mat4 GetLightSpaceMatrix(const float nearPlane, const float farPlane);
        
        utils::Ref<SpecificationVariant> GetSpecification() override { return m_Specification; }
        EnvironmentEntityType GetType() const override { return m_Type; }
	private:
		
        void DrawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader);

        std::shared_ptr<EditorCamera> m_EditorCameraPtr;
        void UBOSetup();

        std::vector<GLuint> visualizerVAOs;
        std::vector<GLuint> visualizerVBOs;
        std::vector<GLuint> visualizerEBOs;

        uint32_t matricesUBO;
        std::vector<glm::mat4> lightMatricesCache;

        EnvironmentEntityType m_Type = EnvironmentEntityType::CascadeShadowMap;
        SpecificationVariant m_Specification = CascadeShadowMapSpecification();

	};
}