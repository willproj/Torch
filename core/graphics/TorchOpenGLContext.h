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

		GBuffer GetGbuffer() { return m_GBuffer; }

		void BindGBuffer() { m_GBuffer.Bind(); }
		void UnbindGBuffer() { m_GBuffer.Unbind(); }

	private:
		TorchModel m_TcModel;
		Shader m_TcShader;
		Shader m_LightingShader;
		Shader m_SkyboxShader;

		GBuffer m_GBuffer;
		RenderQuad m_Quad;

		GLuint m_ScreenFramebuffer;
		GLuint m_EntityIDTexture;
		Window *m_WindowPtr;

		EditorCamera m_EditorCamera;
		Entity m_Entity;

		uint32_t m_ScreenIntTexture;


		Skybox m_Skybox;

		Shader basic;
	};
}