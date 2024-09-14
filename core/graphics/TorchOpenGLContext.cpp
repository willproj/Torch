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

		m_SceneManager = SceneManager::GetSceneManager();
		m_SceneManager->SetCamera(m_EditorCamera);
		
		m_EnvirManager = EnvironmentManager::GetInstance();
		auto atmosphere = std::shared_ptr<EnvironmentEntity>(new AtmosphericScattering(m_EditorCamera));
		auto cascadeShadowMap = std::shared_ptr<EnvironmentEntity>(new CascadeShadowMap(m_EditorCamera));
		auto ssao = std::shared_ptr<EnvironmentEntity>(new SSAO(m_EditorCamera));

		m_EnvirManager->AddEntity(EnvironmentEntityType::Atmosphere, atmosphere);
		m_EnvirManager->AddEntity(EnvironmentEntityType::CascadeShadowMap, cascadeShadowMap);
		m_EnvirManager->AddEntity(EnvironmentEntityType::SSAO, ssao);

		
		test = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/test.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/test.frag");

		vollight = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/volumetricLighting.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/volumetricLighting.frag");

		//light.Initialize();
		shader.InitShader(
			std::string(PROJECT_ROOT) + "/assets/shader/DynamicLightingDataCompute.comp"
		);
		

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
		lightingPassShader.setInt("u_SSAO", 9);

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
		m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::SSAO)->OnUpdate();
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
		//------------------------------------------- renderers & spcifications --------------------------------------------
		auto& atmosphere = m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::Atmosphere)->GetSpecification();
		auto& atmosphereSpcific = std::get<AtmosphericScatteringSpecification>(atmosphere.get());
		auto& cascadeShadowMap = m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::CascadeShadowMap)->GetSpecification();
		auto& shadowMapSpcific = std::get<CascadeShadowMapSpecification>(cascadeShadowMap.get());
		auto& ssao = m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::SSAO)->GetSpecification();
		auto& ssaoSpecific = std::get<SSAOSpecification>(ssao.get());
		//----------------------------------------------------------------------------------------------------------------------

		this->UpdateCameraViewport();
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. Geometry pass (render to GBuffer)
		m_GBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_SceneManager->Render();
		m_GBuffer->Unbind();

		//shader.use();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetGPositionTexture());
		//shader.setInt("gPosition", 0);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetGColorTexture());
		//shader.setInt("gColor", 1);
		//shader.setVec3("lightPosition", glm::vec3(-1.0f, 1.0f, -1.0f));
		//shader.setFloat("lightIntensity", 10);
		//light.Dispatch();


		// render ssao
		m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::SSAO)->BeginRender();
		Texture::BindTexture(0, GL_TEXTURE_2D, m_GBuffer->GetGViewPositionTexture());
		Texture::BindTexture(1, GL_TEXTURE_2D, m_GBuffer->GetGNormalTexture());
		Texture::BindTexture(2, GL_TEXTURE_2D, ssaoSpecific.noiseTexture);
		RenderQuad::Render();
		m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::SSAO)->EndRender();

		m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::SSAO)->Render();
		

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// 2. Render cascade shadow map
		m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::CascadeShadowMap)->BeginRender();
		SceneManager::GetSceneManager()->RenderScene();
		m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::CascadeShadowMap)->EndRender();

		// 3. Lighting pass (render to default framebuffer)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		auto& lightingPassShader = ShaderManager::GetInstance()->GetLightingPassShaderRef();
		lightingPassShader.use();
		lightingPassShader.setVec3("u_CamPos", m_EditorCamera->GetPosition());
		lightingPassShader.setMat4("u_View", m_EditorCamera->GetViewMatrix());
		lightingPassShader.setVec3("u_SunLightDir", atmosphereSpcific.sunPosition);
		lightingPassShader.setVec3("u_SunLightColor", glm::normalize(atmosphereSpcific.finalSunlightColor) * 15.0f);
		lightingPassShader.setFloat("u_FarPlane", m_EditorCamera->GetFarClip());
		lightingPassShader.setInt("u_CascadeCount", shadowMapSpcific.shadowCascadeLevels.size());
		lightingPassShader.setInt("u_TextureSize", shadowMapSpcific.depthMapResolution);

		for (size_t i = 0; i < shadowMapSpcific.shadowCascadeLevels.size(); ++i)
		{
			lightingPassShader.setFloat("u_CascadePlaneDistances[" + std::to_string(i) + "]", shadowMapSpcific.shadowCascadeLevels[i]);
		}

		Texture::BindTexture(0, GL_TEXTURE_2D, m_GBuffer->GetGPositionTexture());
		Texture::BindTexture(1, GL_TEXTURE_2D, m_GBuffer->GetGNormalTexture());
		Texture::BindTexture(2, GL_TEXTURE_2D, m_GBuffer->GetGColorTexture());
		Texture::BindTexture(3, GL_TEXTURE_2D, m_GBuffer->GetGRoughnessAOTexture());
		Texture::BindTexture(4, GL_TEXTURE_CUBE_MAP, ibl.GetIrradianceTexture());
		Texture::BindTexture(5, GL_TEXTURE_CUBE_MAP, ibl.GetPrefilterTexture());
		Texture::BindTexture(6, GL_TEXTURE_2D, ibl.GetBrdfLUTTexture());
		Texture::BindTexture(7, GL_TEXTURE_2D_ARRAY, shadowMapSpcific.shadowMapTexture);
		Texture::BindTexture(8, GL_TEXTURE_2D, m_GBuffer->GetGLightSpacePosition());
		Texture::BindTexture(9, GL_TEXTURE_2D, ssaoSpecific.ssaoColorBlurTexture);
		RenderQuad::Render();
		

		// 4. Blit depth buffer from GBuffer to default framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer->GetFramebufferID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Blit to default framebuffer
		glBlitFramebuffer(0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, 0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind framebuffer
		
		// 5. Render Lights to Light Framebuffer to get light ID
		BindLightIDBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_SceneManager->RenderLightsToID();
		UnbindLightIDBuffer();

		// 6. Render Lights
		m_SceneManager->RenderLights();

		// 7. skybox (to default framebuffer)
		if (m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::Atmosphere)->IsRunning())
		{
			ibl.BindCubemapFramebuffer();
			m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::Atmosphere)->SetShader();
			for (int i = 0; i < 6; i++)
			{
				ibl.RenderAtmosphereToCubemapFace(i);
				m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::Atmosphere)->Render(ibl.views[i], ibl.projection);
			}
			glBindTexture(GL_TEXTURE_CUBE_MAP, ibl.GetCubemapTexture());
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

			ibl.UnbindFramebuffer();

			ibl.RenderIrradianceCubemap();
			ibl.UnbindFramebuffer();

			ibl.RenderPrefilterCubemap();
			ibl.UnbindFramebuffer();
		}

		m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::Atmosphere)->Render();

		// 8. Copy final rendered result from default framebuffer to texture for ImGui
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); // Read from default framebuffer
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ScreenFramebuffer); // Draw to texture
		glBlitFramebuffer(0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, 0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind final framebuffer

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//test.use();
		//test.setInt("tex", 0);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, ssaoSpecific.ssaoColorBlurTexture);
		//RenderQuad::Render();

		//vollight.use();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetGPositionTexture());
		//vollight.setInt("gPosition", 0);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_3D, light.GetLightVolumeTexture());
		//vollight.setInt("lightVolumeTexture", 1);
		//vollight.setVec3("viewPosition", m_EditorCamera->GetPosition());
		//RenderQuad::Render();
		
		auto& god = ShaderManager::GetInstance()->GetGodRayShaderRef();
		god.use();
		god.setVec3("camPos", m_EditorCamera->GetPosition());
		god.setMat4("projection", m_EditorCamera->GetProjection());
		god.setMat4("view", m_EditorCamera->GetViewMatrix());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetGDepthTexture());
		god.setInt("gDepth", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetGPositionTexture());
		god.setInt("gPosition", 1);
		god.setVec3("sunPosition", atmosphereSpcific.sunPosition * 10.0f);
		god.setVec3("sunColor", glm::vec3(1.0f));


		god.setFloat("densityFactor", 0.1f);  // Higher density for more visible rays
		god.setFloat("depthBias", 0.002f);    // Small bias to prevent Z-fighting artifacts
		god.setFloat("maxDistance", 500.0f);  // Longer ray distance for dramatic effect
		god.setFloat("stepSize", 0.2f);

		RenderQuad::Render();

	}

}