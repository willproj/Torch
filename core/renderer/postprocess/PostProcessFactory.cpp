#include "PostProcessFactory.h"

namespace core
{
	PostProcessEffect& PostProcessFactory::GetPostProcessEffect(PostProcessType type)
	{
		// TODO: insert return statement here
		switch (type)
		{
		case core::PostProcessType::SSAO:
			return singleton<SSAO>();
			break;
		case core::PostProcessType::Bloom:
			return singleton<Bloom>();
			break;
		default:
			break;
		}
	}
}