#pragma once
#include <pch/pch.h>
#include "Bloom.h"
#include "core/glcore/Shader.h"

namespace core
{
	class BloomRenderer
	{
	public:
		BloomRenderer() = default;	
		~BloomRenderer() = default;

		void Initialize();
		
		uint32_t& GetSrcTextureRef() { return m_SrcTexture; }
		uint32_t& GetDesTextureRef() { return m_DesTexture; }

		void Create();


		Bloom& GetBloom() { return m_Bloom; }
		void SetSrcTexture(uint32_t tex) { m_SrcTexture = tex; }

	private:
		Bloom m_Bloom;

		glm::ivec2 m_SrcViewportSize;
		glm::vec2 m_SrcViewportSizeFloat;

		uint32_t m_SrcTexture;
		uint32_t m_DesTexture;
		float bloomFilterRadius = 0.005f;
		void RenderDownSample();
		void RenderUpSample();

		float m_Threshold = (1.5);
		float m_Knee = (0.1);
	};
}