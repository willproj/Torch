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

namespace core
{
	class TorchOpenGLContext :public TorchGraphicsContext
	{
	public:
		TorchOpenGLContext();
		~TorchOpenGLContext();

		void DrawFrame();
		void OnUpdate();
		void CreateOffScreenTexture(int width, int height);
		GLuint GetScreenFramebuffer() const { return m_ScreenFramebuffer; }
		APIType GetAPIType() const override { return APIType::OpenGL; }

		GBuffer GetGbuffer() const { return m_GBuffer; }
	private:
		TorchModel m_TcModel;
		Shader m_TcShader;
		Shader m_LightingShader;
		
		GBuffer m_GBuffer;
		RenderQuad m_Quad;

		GLuint m_ScreenFramebuffer;
		GLuint m_EntityIDTexture;
		Window* m_WindowPtr;

		EditorCamera m_EditorCamera;
		Entity m_Entity;
	};
}