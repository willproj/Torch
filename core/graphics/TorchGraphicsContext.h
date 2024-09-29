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

	enum class GBufferRenderType
	{
		All,
		GColor,
		GPosition,
		GNormal,
		GDepth
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
		virtual void OnUpdate(uint32_t width, uint32_t height) {};
		virtual GBuffer& GetGbuffer() { return GBuffer(); }
		virtual GLuint GetScreenFramebuffer() const { return 0; }
		virtual void CreateOffScreenTexture(int width, int height) {};

		virtual void BindGBuffer() {}
		virtual void UnbindGBuffer() {}

		virtual void SetUpRenderType(const GBufferRenderType& type) {}
		virtual GBufferRenderType GetRenderType() { return GBufferRenderType::All; }

		virtual APIType GetAPIType() const = 0;
		GLuint GetScreenTexture() const { return m_ScreenTexture; }

	protected:
		TorchGraphicsContext() noexcept = default;

		static std::unique_ptr<TorchGraphicsContext> s_GraphicsContextInstance;
		GLuint m_ScreenTexture;
	};
}


