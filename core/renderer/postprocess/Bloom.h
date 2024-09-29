#pragma once
#include <pch/pch.h>

namespace core
{
	// bloom stuff
	struct bloomMip
	{
		glm::vec2 size;
		glm::ivec2 intSize;
		uint32_t texture;
	};

	class Bloom
	{
	public:
		Bloom() = default;
		~Bloom() = default;

		void Initialize();
		
		std::vector<bloomMip>& GetMipChain() { return m_MipChain; }

		void Create(uint32_t width, uint32_t height);

	private:
		std::vector<bloomMip> m_MipChain;
		uint32_t m_BloomFramebuffer;
		int32_t m_MipChainLength = 6;

	};
}