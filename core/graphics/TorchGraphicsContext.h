#pragma once
#include <pch/pch.h>

namespace core
{
	class TorchGraphicsContext
	{
	public:
		static std::unique_ptr<TorchGraphicsContext>& GetGraphicsContext();

		TorchGraphicsContext(const TorchGraphicsContext&) = delete;
        TorchGraphicsContext& operator=(const TorchGraphicsContext&) = delete;

        TorchGraphicsContext(TorchGraphicsContext&&) noexcept = default;
        TorchGraphicsContext& operator=(TorchGraphicsContext&&) noexcept = default;

        virtual ~TorchGraphicsContext() = default;
		virtual void DrawFrame() = 0;
		virtual void ReCreate() = 0;

	protected:
		TorchGraphicsContext() noexcept = default;

		static std::unique_ptr<TorchGraphicsContext> s_GraphicsContextInstance;
	};
}


