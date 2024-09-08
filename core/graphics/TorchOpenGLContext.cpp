#include "TorchOpenGLContext.h"
#include <utils/ServiceLocator.h>
#include <editor/Editor.h>
#include "core/renderer/Component.h"
#include "core/renderer/SceneManager.h"
#include "core/renderer/RenderCube.h"
#include "core/glcore/Texture.h"

namespace core
{
	TorchOpenGLContext::TorchOpenGLContext()
	{
		m_ShaderManager = ShaderManager::GetInstance();
		m_ShaderManager->Initialize();

		m_GBuffer = std::make_shared<GBuffer>();
		m_GBuffer->Initialize();
		
		m_WindowPtr = utils::ServiceLocator::GetWindow();
		m_EditorCamera = std::make_shared<EditorCamera>();

		shadowmap.Initialize(m_EditorCamera);
		
		m_SceneManager = SceneManager::GetSceneManager();
		m_SceneManager->SetCamera(m_EditorCamera);
		
		m_EnvirManager = EnvironmentManager::GetInstance();
		auto atmosphere = std::shared_ptr<EnvironmentEntity>(new AtmosphericScattering(m_EditorCamera));
		m_EnvirManager->AddEntity(EnvironmentEntityType::Atmosphere, atmosphere);
		
		test = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/test.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/test.frag");

		debugCascadeShader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/debugcascade.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/debugcascade.frag");

		auto& lightingPassShader = ShaderManager::GetInstance()->GetLightingPassShaderRef();
		lightingPassShader.use();
		lightingPassShader.setInt("gPosition", 0);
		lightingPassShader.setInt("gNormal", 1);
		lightingPassShader.setInt("gAlbedoSpec", 2);
		lightingPassShader.setInt("gRoughAO", 3);
		lightingPassShader.setInt("u_IrradianceMap", 4);
		lightingPassShader.setInt("u_PrefilterMap", 5);
		lightingPassShader.setInt("u_BrdfLUT", 6);
		lightingPassShader.setInt("u_ShadowMap", 7);
		lightingPassShader.setInt("gLightSpacePosition", 8);

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
		RenderQuad::Render();
	}

	void TorchOpenGLContext::RenderGBufferNormalTexture()
	{
		RenderQuad::Render();
	}

	void TorchOpenGLContext::RenderGBufferDepthTexture()
	{
		RenderQuad::Render();
	}

	void TorchOpenGLContext::RenderGBufferColorTexture()
	{
		RenderQuad::Render();
	}

	void TorchOpenGLContext::RenderAllGBufferTextures()
	{
		RenderQuad::Render();
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
		
		shadowmap.UBOSetup();

		shadowmap.Bind();
		glViewport(0, 0, shadowmap.GetDepthMapResolution(), shadowmap.GetDepthMapResolution());
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		SceneManager::GetSceneManager()->RenderScene();
		glCullFace(GL_BACK);
		shadowmap.Unbind();
		

		// 2. Lighting pass (render to default framebuffer)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear default framebuffer for lighting
		auto& lightingPassShader = ShaderManager::GetInstance()->GetLightingPassShaderRef();
		lightingPassShader.use();
		lightingPassShader.setVec3("camPos", m_EditorCamera->GetPosition());
		lightingPassShader.setMat4("view", m_EditorCamera->GetViewMatrix());
		// Pass these as uniforms to the shader
		lightingPassShader.setVec3("u_SunLightDir", std::get<AtmosphericScatteringSpecification>(atmosphere.get()).sunPosition);
		lightingPassShader.setVec3("u_SunLightColor", glm::normalize(std::get<AtmosphericScatteringSpecification>(atmosphere.get()).finalSunlightColor) * 15.0f);
		lightingPassShader.setFloat("farPlane", m_EditorCamera->GetFarClip());
		lightingPassShader.setInt("cascadeCount", m_EditorCamera->shadowCascadeLevels.size());
		for (size_t i = 0; i < m_EditorCamera->shadowCascadeLevels.size(); ++i)
		{
			lightingPassShader.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", m_EditorCamera->shadowCascadeLevels[i]);
		}

		Texture::BindTexture(0, GL_TEXTURE_2D, m_GBuffer->GetGPositionTexture());
		Texture::BindTexture(1, GL_TEXTURE_2D, m_GBuffer->GetGNormalTexture());
		Texture::BindTexture(2, GL_TEXTURE_2D, m_GBuffer->GetGColorTexture());
		Texture::BindTexture(3, GL_TEXTURE_2D, m_GBuffer->GetGRoughnessAOTexture());
		Texture::BindTexture(4, GL_TEXTURE_CUBE_MAP, ibl.GetIrradianceTexture());
		Texture::BindTexture(5, GL_TEXTURE_CUBE_MAP, ibl.GetPrefilterTexture());
		Texture::BindTexture(6, GL_TEXTURE_2D, ibl.GetBrdfLUTTexture());
		Texture::BindTexture(7, GL_TEXTURE_2D_ARRAY, shadowmap.GetShadowMapTexture());
		Texture::BindTexture(8, GL_TEXTURE_2D, m_GBuffer->GetGLightSpacePosition());
		RenderQuad::Render();

		


		// debug shadow map
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//
		//vollight.use();
		//vollight.setInt("gDepth", 0); // Texture unit 0 for depth texture
		//vollight.setFloat("near_plane", m_EditorCamera->GetNearClip()); // Texture unit 0 for depth texture
		//vollight.setFloat("far_plane", 10); // Texture unit 0 for depth texture
		//
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D_ARRAY, shadowmap.GetShadowMapTexture());
		//RenderQuad::Render();

		// 3. Blit depth buffer from GBuffer to default framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer->GetFramebufferID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Blit to default framebuffer
		glBlitFramebuffer(0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, 0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind framebuffer
		
		// Render Lights to Light Framebuffer to get light ID
		BindLightIDBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_SceneManager->RenderLightsToID();
		UnbindLightIDBuffer();

		// Render Lights
		m_SceneManager->RenderLights();

		// 4. skybox (to default framebuffer)
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


		//shadowmap.lightMatricesCache = shadowmap.getLightSpaceMatrices();
		//if (shadowmap.lightMatricesCache.size() != 0)
		//{
		//	glEnable(GL_BLEND);
		//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//	debugCascadeShader.use();
		//	debugCascadeShader.setMat4("projection", m_EditorCamera->getProjection());
		//	debugCascadeShader.setMat4("view", m_EditorCamera->GetViewMatrix());
		//	shadowmap.drawCascadeVolumeVisualizers(shadowmap.lightMatricesCache, &debugCascadeShader);
		//	glDisable(GL_BLEND);
		//}

		m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::Atmosphere)->Render();


		// 5. Copy final rendered result from default framebuffer to texture for ImGui
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); // Read from default framebuffer
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ScreenFramebuffer); // Draw to texture
		glBlitFramebuffer(0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, 0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind final framebuffer
	}

}