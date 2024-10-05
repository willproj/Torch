#pragma once
#include <pch.h>
#include <utils/TemplateUtils.h>
#include "core/renderer/camera/EditorCamera.h"

namespace core
{
	struct BloomMipTexture
	{
		glm::vec2 size;
		glm::ivec2 intSize;
		uint32_t texture;
	};

	struct BloomSpecification
	{
		std::vector<BloomMipTexture> m_MipTextureChain;
		int32_t m_MipChainLength = 6;

		glm::ivec2 m_SrcViewportSize;
		glm::vec2 m_SrcViewportSizeFloat;

		float bloomFilterRadius = 0.005f;

		uint32_t m_BloomBlurTexture;
		uint32_t m_SrcTexture;
	};

	struct SSAOSpecification
	{
		uint32_t ssaoFramebuffer;
		uint32_t ssaoBlurFramebuffer;
		uint32_t ssaoColorTexture;
		uint32_t ssaoColorBlurTexture;
		std::vector<glm::vec3> ssaoKernel;
		uint32_t noiseTexture;
		std::shared_ptr<EditorCamera> camera;
	};

	using PostProcessSpeciVariant = std::variant
	<
		BloomSpecification,
		SSAOSpecification
	>;

	class PostProcessEffect
	{
	public:
		virtual ~PostProcessEffect() = default;

		virtual void Initialize(uint32_t width, uint32_t height) = 0;
		virtual void OnUpdate(uint32_t width, uint32_t height) = 0;

		virtual void BeginRender() = 0; 
		virtual void EndRender() = 0;
		
		virtual uint32_t GetNoise() { return 0; }

		virtual PostProcessSpeciVariant& GetSpecification() = 0;
	};
}