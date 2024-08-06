#pragma once
#include <pch/pch.h>

namespace core
{
	class TorchGraphicsContext
	{
	public:
		static std::unique_ptr<TorchGraphicsContext> GetGraphicsContext();
	protected:
		TorchGraphicsContext();
	};
}


