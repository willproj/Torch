#pragma once
#include <pch/pch.h>
#include "EnvironmentEntity.h"
#include "core/renderer/camera/EditorCamera.h"

namespace core
{
	class SSAO : public EnvironmentEntity
	{
	public:
		SSAO() = default;
		~SSAO() = default;

		void Initialize() override;
		SSAO(std::shared_ptr<EditorCamera> camera);

		void Render() override;
		void BeginRender() override;
		void EndRender() override;

		void OnUpdate() override;

		utils::Ref<SpecificationVariant> GetSpecification() override { return m_Specification; }
		EnvironmentEntityType GetType() const override { return m_Type; }

	private:
		std::shared_ptr<EditorCamera> m_EditorCameraPtr;
		EnvironmentEntityType m_Type = EnvironmentEntityType::SSAO;
		SpecificationVariant m_Specification = SSAOSpecification();

		float ourLerp(float a, float b, float f)
		{
			return a + f * (b - a);
		}

		void GenerateNoiseTexture();

	};
}