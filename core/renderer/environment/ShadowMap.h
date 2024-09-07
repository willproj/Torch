#pragma once

namespace core
{
	class ShadowMap
	{
	public:
		ShadowMap();
		~ShadowMap();
		void Initialize();

		void Bind();
		void Unbind();

		uint32_t GetShadowMapTexture() const { return m_ShadowMapTexture; }
	private:
		uint32_t m_ShadowMapFramebuffer;
		uint32_t m_ShadowMapWidth, m_ShadowMapHeight;
		uint32_t m_ShadowMapTexture;

	};
}