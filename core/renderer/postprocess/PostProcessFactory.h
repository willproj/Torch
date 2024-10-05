#pragma once
#include "Bloom.h"
#include "SSAO.h"

namespace core
{

	enum class PostProcessType
	{
		SSAO,
		Bloom
	};

	class PostProcessFactory
	{
	public:
		PostProcessEffect& GetPostProcessEffect(PostProcessType type);
	private:
		PostProcessFactory() = default;
		template <typename T>
		friend T& singleton();
	};
}