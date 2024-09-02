#pragma once
#include <pch/pch.h>
#include "TorchGraphicsContext.h"
#include "core/glcore/TorchModel.h"
#include "core/glcore/Shader.h"
#include "core/glcore/GBuffer.h"
#include "core/renderer/RenderQuad.h"
#include "core/platform/TorchWindow.h"
#include "core/renderer/camera/EditorCamera.h"
#include "core/renderer/Entity.h"
#include "core/renderer/skybox/Skybox.h"
#include "core/renderer/EnvironmentManager.h"
#include "core/renderer/SceneManager.h"
#include "core/renderer/environment/HDR.h"

namespace core
{


	class TorchOpenGLContext : public TorchGraphicsContext
	{
	public:
		TorchOpenGLContext();
		~TorchOpenGLContext();

		void DrawFrame();
		void OnUpdate();
		void CreateOffScreenTexture(int width, int height);
		GLuint GetScreenFramebuffer() const { return m_ScreenFramebuffer; }
		APIType GetAPIType() const override { return APIType::OpenGL; }

		GBuffer& GetGbuffer() override { return *m_GBuffer; }

		void BindGBuffer() { m_GBuffer->Bind(); }
		void UnbindGBuffer() { m_GBuffer->Unbind(); }

		void UpdateCameraViewport();

		void SetUpRenderType(const GBufferRenderType& type) override;
		GBufferRenderType GetRenderType() override { return m_RenderType; }

	private:
		void RenderGBufferPositionTexture();
		void RenderGBufferNormalTexture();
		void RenderGBufferDepthTexture();
		void RenderGBufferColorTexture();
		void RenderAllGBufferTextures();


		Shader m_LightingShader;

		std::shared_ptr<GBuffer> m_GBuffer;
		RenderQuad m_Quad;

		GLuint m_ScreenFramebuffer;
		GLuint m_EntityIDTexture;
		Window *m_WindowPtr;

		std::shared_ptr<EditorCamera> m_EditorCamera;

		uint32_t m_ScreenIntTexture;

		std::shared_ptr<EnvironmentManager> m_EnvirManager;
		std::shared_ptr<SceneManager> m_SceneManager;

		GBufferRenderType m_RenderType = GBufferRenderType::All;

		HDR m_HDR;
	};
}