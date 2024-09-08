#pragma once
#include <pch/pch.h>
#include "core/glcore/Shader.h"
#include "EnvironmentEntity.h"
#include "core/glcore/TorchModel.h"

namespace core
{
	class AtmosphericScattering : public EnvironmentEntity
	{
	public:
		AtmosphericScattering(std::shared_ptr<EditorCamera> camera);
		AtmosphericScattering() = default;
		~AtmosphericScattering() = default;

		void Render() override;
		void OnUpdate() override;
		void Render(const glm::mat4& view, const glm::mat4& projection) override;
		void SetShader() override;
		bool IsRunning() override;
		void SetIsRunning(bool isRunning) override;

		utils::Ref<SpecificationVariant> GetSpecification() override { return m_Specification; }
		EnvironmentEntityType GetType() const override { return m_Type; }
	private:
		void Initialize();
		void SetDefault();
		void RenderAtmosphere();
		SpecificationVariant m_Specification = AtmosphericScatteringSpecification();
		std::shared_ptr<TorchModel> m_SkyboxSphere;

		GLuint ssbo;

		bool m_IsRunning = false;
	};
}

