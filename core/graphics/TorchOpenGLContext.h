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
#include "core/renderer/postprocess/IBL.h"
#include "core/renderer/environment/ShadowMap.h"

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

		virtual void BindLightIDBuffer() { glBindFramebuffer(GL_FRAMEBUFFER, m_LightFramebuffer); };
		virtual void UnbindLightIDBuffer() { glBindFramebuffer(GL_FRAMEBUFFER, 0); };

	private:
		void RenderGBufferPositionTexture();
		void RenderGBufferNormalTexture();
		void RenderGBufferDepthTexture();
		void RenderGBufferColorTexture();
		void RenderAllGBufferTextures();

		void CreateLightIDTexture(int width, int height);

		Shader m_LightingShader;

		std::shared_ptr<GBuffer> m_GBuffer;
		RenderQuad m_Quad;

		GLuint m_ScreenFramebuffer;
		GLuint m_EntityIDTexture;

		GLuint m_LightFramebuffer;
		GLuint m_LightIDTexture;


		Window *m_WindowPtr;

		std::shared_ptr<EditorCamera> m_EditorCamera;

		uint32_t m_ScreenIntTexture;

		std::shared_ptr<EnvironmentManager> m_EnvirManager;
		std::shared_ptr<SceneManager> m_SceneManager;

		GBufferRenderType m_RenderType = GBufferRenderType::All;

		HDR m_HDR;
		ShadowMap shadowmap;
		Shader simpleshader;

		// lights
		// ------
		std::vector<glm::vec3> lightPositions = {
			glm::vec3(0.0f, 0.0f, 10.0f),
		};
		std::vector<glm::vec3> lightColors = {
			glm::vec3(150.0f, 150.0f, 150.0f),
		};
		int nrRows = 7;
		int nrColumns = 7;
		float spacing = 2.5;

		IBL ibl;
		Shader test;


		bool iblonce = true;

		Shader vollight;
	};
}