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

		/*uint32_t ssaoFramebuffer;
		uint32_t ssaoBlurFramebuffer;
		uint32_t ssaoColorTexture;
		uint32_t ssaoColorBlurTexture;
		std::vector<glm::vec3> ssaoKernel;
		uint32_t noiseTexture;*/


		void Initialize(uint32_t width, uint32_t height) override;
		float Lerp(float a, float b, float f)
		{
			return a + f * (b - a);
		}

		//std::shared_ptr<EditorCamera> m_Camera;

		void GenerateNoiseTexture();

		template <typename T>
		friend T& singleton();

		PostProcessSpeciVariant m_Specification = SSAOSpecification();
	};
}