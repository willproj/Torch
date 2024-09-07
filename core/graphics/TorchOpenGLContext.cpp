#include "TorchOpenGLContext.h"
#include <utils/ServiceLocator.h>
#include <editor/Editor.h>
#include "core/renderer/Component.h"
#include "core/renderer/SceneManager.h"
#include "core/renderer/RenderCube.h"

namespace core
{
	TorchOpenGLContext::TorchOpenGLContext()
	{
		m_GBuffer = std::make_shared<GBuffer>();
		m_GBuffer->Initialize();
		m_WindowPtr = utils::ServiceLocator::GetWindow();
		m_EditorCamera = std::make_shared<EditorCamera>();
		
		m_SceneManager = SceneManager::GetSceneManager();
		m_SceneManager->SetCamera(m_EditorCamera);
		
		m_EnvirManager = EnvironmentManager::GetInstance();
		auto atmosphere = std::shared_ptr<EnvironmentEntity>(new AtmosphericScattering(m_EditorCamera));
		m_EnvirManager->AddEntity(EnvironmentEntityType::Atmosphere, atmosphere);
			

		m_LightingShader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/lighting.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/lighting.frag");

		test = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/test.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/test.frag");

		vollight = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/lighting.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/volumetricLighting.frag");

		simpleshader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/shadowMapDepth.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/shadowMapDepth.frag");



		m_LightingShader.use();
		m_LightingShader.setInt("gPosition", 0);
		m_LightingShader.setInt("gNormal", 1);
		m_LightingShader.setInt("gAlbedoSpec", 2);
		m_LightingShader.setInt("gRoughAO", 3);
		m_LightingShader.setInt("u_IrradianceMap", 4);
		m_LightingShader.setInt("u_PrefilterMap", 5);
		m_LightingShader.setInt("u_BrdfLUT", 6);
		m_LightingShader.setInt("u_ShadowMap", 7);
		m_LightingShader.setInt("gLightSpacePosition", 8);

		SceneManager::GetSceneManager()->GetSceneRef()->CreateEntity();
		CreateOffScreenTexture(m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height);
		CreateLightIDTexture(m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height);

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

	void TorchOpenGLContext::CreateLightIDTexture(int width, int height)
	{
		// Create the default color texture
		glGenTextures(1, &m_LightIDTexture);
		glBindTexture(GL_TEXTURE_2D, m_LightIDTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Create framebuffer
		glGenFramebuffers(1, &m_LightFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_LightFramebuffer);

		// Attach the color texture to the framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_LightIDTexture, 0);

		// Check if the framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	

	void TorchOpenGLContext::OnUpdate()
	{
		m_GBuffer->OnUpdate();

		if (m_ScreenFramebuffer)
		{
			glDeleteFramebuffers(1, &m_ScreenFramebuffer);
		}

		if (m_ScreenTexture)
		{
			glDeleteTextures(1, &m_ScreenTexture);
		}

		if (m_LightFramebuffer)
		{
			glDeleteFramebuffers(1, &m_LightFramebuffer);
		}

		if (m_LightIDTexture)
		{
			glDeleteTextures(1, &m_LightIDTexture);
		}

		CreateOffScreenTexture(m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height);
		CreateLightIDTexture(m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height);
		m_HDR.OnUpdate();
	}

	TorchOpenGLContext::~TorchOpenGLContext()
	{
	}

	void TorchOpenGLContext::UpdateCameraViewport()
	{
		m_EditorCamera->Update();
		auto& viewport = editor::Editor::GetEditorModule(editor::EditorType::Viewport);
		glm::vec2 viewportSize = viewport->GetWindowContentSize();
		m_EditorCamera->SetViewportSize(viewportSize.x, viewportSize.y);
	}

	void TorchOpenGLContext::SetUpRenderType(const GBufferRenderType& type)
	{
		m_RenderType = type;
	}

	void TorchOpenGLContext::RenderGBufferPositionTexture()
	{
		m_GBuffer->BindPositionTexture();  // World positions
		m_Quad.renderQuad();
	}

	void TorchOpenGLContext::RenderGBufferNormalTexture()
	{
		m_GBuffer->BindNormalTexture();    // Normals
		m_Quad.renderQuad();
	}

	void TorchOpenGLContext::RenderGBufferDepthTexture()
	{
		m_GBuffer->BindDepthTexture();  // World positions
		m_Quad.renderQuad();
	}

	void TorchOpenGLContext::RenderGBufferColorTexture()
	{
		m_GBuffer->BindColorTexture();     // Color + Specular
		m_Quad.renderQuad();
	}

	void TorchOpenGLContext::RenderAllGBufferTextures()
	{
		m_GBuffer->BindColorTexture();     // Color + Specular
		m_GBuffer->BindNormalTexture();    // Normals
		m_GBuffer->BindPositionTexture();  // World positions
		m_GBuffer->BindDepthTexture();  // World positions
		m_Quad.renderQuad();
	}
	

	void TorchOpenGLContext::DrawFrame()
	{
		this->UpdateCameraViewport();
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. Geometry pass (render to GBuffer)
		m_GBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_SceneManager->Render();
		m_GBuffer->Unbind();


		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		auto atmosphere = m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::Atmosphere)->GetSpecification();
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 7.5f;
		//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(std::get<AtmosphericScatteringSpecification>(atmosphere.get()).sunPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		simpleshader.use();
		simpleshader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, 1024, 1024);
		shadowmap.Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		SceneManager::GetSceneManager()->RenderScene(simpleshader);
		glCullFace(GL_BACK);
		shadowmap.Unbind();
		

		// 2. Lighting pass (render to default framebuffer)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear default framebuffer for lighting

		m_LightingShader.use();
		m_LightingShader.setVec3("camPos", m_EditorCamera->GetPosition());
		// Pass these as uniforms to the shader
		m_LightingShader.setVec3("u_SunLightDir", std::get<AtmosphericScatteringSpecification>(atmosphere.get()).sunPosition);
		m_LightingShader.setVec3("u_SunLightColor", glm::normalize(std::get<AtmosphericScatteringSpecification>(atmosphere.get()).finalSunlightColor) * 15.0f);

		m_GBuffer->BindPositionTexture();  // World positions
		m_GBuffer->BindNormalTexture();    // Normals
		m_GBuffer->BindColorTexture();     // Color + Specular
		m_GBuffer->BindRoughnessAOTexture();  // World positions
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ibl.GetIrradianceTexture());
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ibl.GetPrefilterTexture());
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, ibl.GetBrdfLUTTexture());
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, shadowmap.GetShadowMapTexture());
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetGLightSpacePosition());
		m_Quad.renderQuad();



		// debug shadow map
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//
		//vollight.use();
		//vollight.setInt("gDepth", 0); // Texture unit 0 for depth texture
		//vollight.setFloat("near_plane", m_EditorCamera->GetNearClip()); // Texture unit 0 for depth texture
		//vollight.setFloat("far_plane", 10); // Texture unit 0 for depth texture
		//
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, shadowmap.GetShadowMapTexture());
		//
		//m_Quad.renderQuad();



		// 3. Blit depth buffer from GBuffer to default framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer->GetFramebufferID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Blit to default framebuffer
		glBlitFramebuffer(0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, 0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind framebuffer

		
		

		BindLightIDBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_SceneManager->RenderLightsToID();
		UnbindLightIDBuffer();

		m_SceneManager->RenderLights();

		// 4. Render scene model and skybox (to default framebuffer)
		// - Render scene model
		if (m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::Atmosphere)->IsRunning())
		{
			ibl.BindCubemapFramebuffer();
			m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::Atmosphere)->SetShader();
			for (int i = 0; i < 6; i++)
			{
				ibl.RenderAtmosphereToCubemapFace(i);
				m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::Atmosphere)->Render(ibl.views[i], ibl.projection);
			}
			ibl.UnbindFramebuffer();

			ibl.RenderIrradianceCubemap();
			ibl.UnbindFramebuffer();

			ibl.RenderPrefilterCubemap();
			ibl.UnbindFramebuffer();
		}

		m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::Atmosphere)->Render();

		

		// 5. Copy final rendered result from default framebuffer to texture for ImGui
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); // Read from default framebuffer
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ScreenFramebuffer); // Draw to texture
		glBlitFramebuffer(0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, 0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind final framebuffer
		
		
	}

}