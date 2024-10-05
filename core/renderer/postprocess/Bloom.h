#pragma once
#include "PostProcessEffect.h"

namespace core
{
	

	class Bloom : public PostProcessEffect
	{
	public:
		~Bloom() override;
		
		void BeginRender() override;
		void EndRender() override;

		//uint32_t GetPostProcessTexture() override { return m_MipTextureChain[0].texture; }
		void OnUpdate(uint32_t width, uint32_t height) override;

		PostProcessSpeciVariant& GetSpecification() { return m_Specification; }
	private:
		Bloom();

		void Initialize(uint32_t width, uint32_t height) override;
		
		/*std::vector<BloomMipTexture> m_MipTextureChain;
		int32_t m_MipChainLength = 6;

		glm::ivec2 m_SrcViewportSize;
		glm::vec2 m_SrcViewportSizeFloat;

		float bloomFilterRadius = 0.005f;

		uint32_t m_BloomBlurTexture;
		uint32_t m_SrcTexture;*/

		void RenderDownSample();
		void RenderUpSample();

		template <typename T>
		friend T& singleton();

		PostProcessSpeciVariant m_Specification = BloomSpecification();
	};
}