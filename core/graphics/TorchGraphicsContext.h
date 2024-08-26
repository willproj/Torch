#pragma once
#include <pch/pch.h>
#include "core/glcore/GBuffer.h"

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
		virtual void OnUpdate() {};
		virtual GBuffer GetGBuffer() const { return GBuffer(); }
		virtual GLuint GetScreenFramebuffer() const { return 0; }
		virtual void CreateOffScreenTexture(int width, int height) {};

		virtual void BindGBuffer() {}
		virtual void UnbindGBuffer() {}

		virtual APIType GetAPIType() const = 0;
		GLuint GetScreenTexture() const { return m_ScreenTexture; }

	protected:
		TorchGraphicsContext() noexcept = default;

		static std::unique_ptr<TorchGraphicsContext> s_GraphicsContextInstance;
		GLuint m_ScreenTexture;
	};
}


