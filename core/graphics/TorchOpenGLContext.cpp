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
		const std::string filepath = "/assets/models/sphere/scene.gltf";
		m_TcModel.LoadModel(filepath);

		m_TcShader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/shader.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/shader.frag");

		m_LightingShader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/lighting.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/lighting.frag");

		m_Shader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/atmosphere.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/atmosphere.frag");


		m_LightingShader.use();
		m_LightingShader.setInt("gPosition", 0);
		m_LightingShader.setInt("gNormal", 1);
		m_LightingShader.setInt("gColorSpec", 2);

		m_Entity = SceneManager::GetSceneManager()->GetSceneRef()->CreateEntity();

		CreateOffScreenTexture(m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height);
	}

	void TorchOpenGLContext::CreateOffScreenTexture(int width, int height)
	{
		// Create the default color texture
		glGenTextures(1, &m_ScreenTexture);
		glBindTexture(GL_TEXTURE_2D, m_ScreenTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		// Create framebuffer
		glGenFramebuffers(1, &m_ScreenFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenFramebuffer);

		// Attach the color texture to the framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ScreenTexture, 0);

		// Check if the framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

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

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// geometry pass
		m_GBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_TcShader.use();
		// Pass matrices to the shader
		auto &viewport = editor::Editor::GetEditorModule(editor::EditorType::Viewport);
		glm::vec2 viewportSize = viewport->GetWindowContentSize();
		m_EditorCamera.SetViewportSize(viewportSize.x, viewportSize.y);
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 6360, 0.0f));
		m_TcShader.setMat4("model", modelMatrix);
		m_TcShader.setMat4("view", m_EditorCamera.GetViewMatrix());
		m_TcShader.setMat4("projection", m_EditorCamera.getProjection());
		m_TcModel.RenderModel();
		m_GBuffer.Unbind();

		// lighting pass
		glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenFramebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(6420., 6420., 6420.));
		
		m_LightingShader.use();
		m_LightingShader.setVec3("viewPos", m_EditorCamera.GetPosition());
		m_GBuffer.BindColorTexture();
		m_GBuffer.BindNormalTexture();
		m_GBuffer.BindPositionTexture();
		m_Quad.renderQuad();
		
		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL); 
		m_Shader.use();
		m_Shader.setMat4("model", model);
		m_Shader.setMat4("view", m_EditorCamera.GetViewMatrix());
		m_Shader.setMat4("projection", m_EditorCamera.getProjection());
		m_Shader.setVec3("viewPos", m_EditorCamera.GetPosition());
		atmosphere.draw(0.01, m_Shader);
		m_TcModel.RenderModel();
		glDepthFunc(GL_LESS);

		

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
}