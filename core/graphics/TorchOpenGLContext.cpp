#include "TorchOpenGLContext.h"
#include <utils/ServiceLocator.h>
#include <editor/Editor.h>
#include "core/renderer/Component.h"
#include "core/renderer/SceneManager.h"

namespace core
{
	TorchOpenGLContext::TorchOpenGLContext()
	{
		m_WindowPtr = utils::ServiceLocator::GetWindow();
		const std::string filepath = "/assets/models/cube/scene.gltf";
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

		m_Entity = SceneManager::GetSceneManager()->GetSceneRef()->CreateEntity();

		CreateOffScreenTexture(m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height);
	}

	void TorchOpenGLContext::CreateOffScreenTexture(int width, int height)
	{
		glGenTextures(1, &m_ScreenTexture);
		glBindTexture(GL_TEXTURE_2D, m_ScreenTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		GLuint redtexture;
		glGenTextures(1, &m_EntityIDTexture);
		glBindTexture(GL_TEXTURE_2D, m_EntityIDTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		glGenFramebuffers(1, &m_ScreenFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenFramebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ScreenTexture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_EntityIDTexture, 0);
		GLenum buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, buffers);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void TorchOpenGLContext::OnUpdate()
	{
		m_GBuffer.OnUpdate();

		if (m_ScreenFramebuffer)
		{
			glDeleteFramebuffers(1, &m_ScreenFramebuffer);
		}

		if (m_ScreenTexture)
		{
			glDeleteTextures(1, &m_ScreenTexture);
		}

		CreateOffScreenTexture(m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height);
	}

	TorchOpenGLContext::~TorchOpenGLContext()
	{
	}

	void TorchOpenGLContext::DrawFrame()
	{
		m_EditorCamera.Update();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// geometry pass
		m_GBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_TcShader.use();
		// Pass matrices to the shader
		auto& viewport = editor::Editor::GetEditorModule(editor::EditorType::Viewport);
		glm::vec2 viewportSize = viewport->GetWindowContentSize();
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		m_EditorCamera.SetViewportSize(viewportSize.x, viewportSize.y);
		m_TcShader.setMat4("model", modelMatrix);
		m_TcShader.setMat4("view", m_EditorCamera.GetViewMatrix());
		m_TcShader.setMat4("projection", m_EditorCamera.getProjection());
		
		SceneManager::GetSceneManager()->Render(m_TcShader);
		//m_TcModel.RenderModel();
		m_GBuffer.Unbind();

		// lighting pass
		glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenFramebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_LightingShader.use();
		m_GBuffer.BindColorTexture();
		m_GBuffer.BindNormalTexture();
		m_GBuffer.BindPositionTexture();

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_EntityIDTexture);
		m_LightingShader.setInt("gEntityID", 3);

		m_Quad.renderQuad();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}