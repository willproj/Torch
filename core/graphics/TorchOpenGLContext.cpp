#include "TorchOpenGLContext.h"
#include <utils/ServiceLocator.h>
#include <editor/Editor.h>
#include "core/renderer/Component.h"
#include "core/renderer/SceneManager.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

		m_SkyboxShader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/skybox.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/skybox.frag");

		basic = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/test.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/test.frag");

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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT,	 NULL);
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
		auto& viewport = editor::Editor::GetEditorModule(editor::EditorType::Viewport);
		glm::vec2 viewportSize = viewport->GetWindowContentSize();
		m_EditorCamera.SetViewportSize(viewportSize.x, viewportSize.y);

		// 1. Geometry pass (render to GBuffer)
		m_GBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_TcShader.use();
		m_TcShader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f,-2.0f,-2.0f)));
		m_TcShader.setMat4("view", m_EditorCamera.GetViewMatrix());
		m_TcShader.setMat4("projection", m_EditorCamera.getProjection());
		SceneManager::GetSceneManager()->Render(m_TcShader); // Render all scene objects to GBuffer
		m_GBuffer.Unbind();

		// 2. Lighting pass (render to default framebuffer)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear default framebuffer for lighting
		m_LightingShader.use();
		m_LightingShader.setVec3("viewPos", m_EditorCamera.GetPosition());

		// Bind GBuffer textures for lighting calculations
		m_GBuffer.BindColorTexture();     // Color + Specular
		m_GBuffer.BindNormalTexture();    // Normals
		m_GBuffer.BindPositionTexture();  // World positions
		m_Quad.renderQuad();              // Render fullscreen quad for lighting pass

		// 3. Blit depth buffer from GBuffer to default framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer.GetFramebufferID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Blit to default framebuffer
		glBlitFramebuffer(0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, 0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind framebuffer

		// 4. Render scene model and skybox (to default framebuffer)
		// - Render scene model
		basic.use();
		basic.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -0.5f)));
		basic.setMat4("view", m_EditorCamera.GetViewMatrix());
		basic.setMat4("projection", m_EditorCamera.getProjection());
		m_TcModel.RenderModel();

		glDepthFunc(GL_LEQUAL);
		float elapsedTime = static_cast<float>(glfwGetTime());

		// Calculate the sun direction for a day-night cycle
		float angle = fmod(elapsedTime / 60.0f * 2.0f * M_PI, 2.0f * M_PI); // Rotate full circle
		glm::vec3 sunDirection = glm::vec3(
			cos(angle),   // X component
			sin(angle),   // Y component
			-0.5f         // Z component
		);

		// Normalize the sun direction
		sunDirection = glm::normalize(sunDirection);

		// Use the shader program
		m_SkyboxShader.use();

		// Set view and projection matrices
		m_SkyboxShader.setMat4("view", glm::mat4(glm::mat3(m_EditorCamera.GetViewMatrix())));
		m_SkyboxShader.setMat4("projection", m_EditorCamera.getProjection());

		// Set the sun direction uniform
		m_SkyboxShader.setVec3("u_SunDirection", sunDirection);
		m_TcModel.RenderModel();
		//glBindVertexArray(m_Skybox.skyboxVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36); // Render skybox
		//glBindVertexArray(0);
		glDepthFunc(GL_LESS);  // Reset depth function to default

		// 5. Copy final rendered result from default framebuffer to texture for ImGui
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); // Read from default framebuffer
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ScreenFramebuffer); // Draw to texture
		glBlitFramebuffer(0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, 0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind final framebuffer
	}

}