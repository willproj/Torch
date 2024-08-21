#pragma once
#include <pch/pch.h>

namespace core
{
	enum class APIType
	{
		None = 0, // Undefined
		OpenGL,
		Vulkan
	};

	class TorchGraphicsContext
	{
	public:
		static std::unique_ptr<TorchGraphicsContext>& GetGraphicsContext();

		TorchGraphicsContext(const TorchGraphicsContext&) = delete;
        TorchGraphicsContext& operator=(const TorchGraphicsContext&) = delete;

        TorchGraphicsContext(TorchGraphicsContext&&) noexcept = default;
        TorchGraphicsContext& operator=(TorchGraphicsContext&&) noexcept = default;

        virtual ~TorchGraphicsContext() = default;
		virtual void DrawFrame() {};
		virtual void ReCreate() {};

		virtual APIType GetAPIType() const = 0;

	protected:
		TorchGraphicsContext() noexcept = default;

		static std::unique_ptr<TorchGraphicsContext> s_GraphicsContextInstance;
	};
}


