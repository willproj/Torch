#pragma once
#include <pch/pch.h>
#include "core/renderer/camera/EditorCamera.h"
#include "PostProcessEffect.h"

namespace core
{
	class SSAO : public PostProcessEffect
	{
	public:
		~SSAO() override;
		void BeginRender() override;
		void EndRender() override;

		void OnUpdate(uint32_t width, uint32_t height) override;
		PostProcessSpeciVariant& GetSpecification() override { return m_Specification; }

	private:
		SSAO();
		void Initialize(uint32_t width, uint32_t height) override;
		float Lerp(float a, float b, float f)
		{
			return a + f * (b - a);
		}

		void GenerateNoiseTexture();

		template <typename T>
		friend T& singleton();

		PostProcessSpeciVariant m_Specification = SSAOSpecification();
	};
}