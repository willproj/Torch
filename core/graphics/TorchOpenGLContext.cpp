#include "TorchOpenGLContext.h"

namespace core
{
	TorchOpenGLContext::TorchOpenGLContext()
	{
		const std::string filepath = std::string(PROJECT_ROOT) + "/assets/models/sphere/sphere.gltf";
		m_TcModel.LoadModel(filepath);

		m_TcShader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/shader.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/shader.frag");

		m_LightingShader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/lighting.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/lighting.frag");

		m_LightingShader.use();
		m_LightingShader.setInt("gPosition", 0);
		m_LightingShader.setInt("gNormal", 1);
		m_LightingShader.setInt("gColorSpec", 2);


		glGenTextures(1, &m_ScreenTexture);
		glBindTexture(GL_TEXTURE_2D, m_ScreenTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 720, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);


		glGenFramebuffers(1, &m_ScreenFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenFramebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ScreenTexture, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	TorchOpenGLContext::~TorchOpenGLContext()
	{
	}

	void TorchOpenGLContext::DrawFrame()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// geometry pass
		m_GBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_TcShader.use();
		// Pass matrices to the shader
		m_TcShader.setMat4("model", modelMatrix);
		m_TcShader.setMat4("view", viewMatrix);
		m_TcShader.setMat4("projection", projectionMatrix);
		m_TcModel.RenderModel();
		m_GBuffer.Unbind();

		// lighting pass
		glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenFramebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_LightingShader.use();
		m_GBuffer.BindColorTexture();
		m_GBuffer.BindNormalTexture();
		m_GBuffer.BindPositionTexture();
		m_Quad.renderQuad();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}