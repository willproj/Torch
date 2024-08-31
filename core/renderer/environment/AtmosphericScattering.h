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
		utils::Ref<SpecificationVariant> GetSpecification() override { return m_Specification; }
		EnvironmentEntityType GetType() const override { return m_Type; }

	private:
		void Initialize();
		Shader m_Shader;
		SpecificationVariant m_Specification = AtmosphericScatteringSpecification();
		std::shared_ptr<TorchModel> m_SkyboxSphere;
	};
}

