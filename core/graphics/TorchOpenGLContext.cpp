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

		m_EnvirManager->AddEntity(EnvironmentEntityType::Atmosphere, atmosphere);
		m_EnvirManager->AddEntity(EnvironmentEntityType::CascadeShadowMap, cascadeShadowMap);
		

		test = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/test.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/test.frag");

		auto& lightingPassShader = ShaderManager::GetInstance()->GetLightingPassShaderRef();
		lightingPassShader.use();
		lightingPassShader.setInt("gPosition", 1);
		lightingPassShader.setInt("gNormal", 2);
		lightingPassShader.setInt("gAlbedoSpec", 3);
		lightingPassShader.setInt("gRoughAO", 4);
		lightingPassShader.setInt("u_IrradianceMap", 5);
		lightingPassShader.setInt("u_PrefilterMap", 6);
		lightingPassShader.setInt("u_BrdfLUT", 7);
		lightingPassShader.setInt("u_ShadowMap", 8);
		lightingPassShader.setInt("gLightSpacePosition", 9);
		lightingPassShader.setInt("u_SSAO", 10);
		lightingPassShader.setInt("u_SkyboxMap", 11);
		lightingPassShader.setInt("gDepth", 12);

		SceneManager::GetSceneManager()->GetSceneRef()->CreateEntity();
		CreateOffScreenTexture(m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height);
		CreateSkyboxTexture(m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height);
		CreatePostprocessTextures(m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height);

		auto& factory = singleton<PostProcessFactory>();
		auto& bloomEffect = factory.GetPostProcessEffect(PostProcessType::Bloom);
		auto& ssaoEffect = factory.GetPostProcessEffect(PostProcessType::SSAO);
		std::get<SSAOSpecification>(ssaoEffect.GetSpecification()).camera = m_EditorCamera;
	}

	void TorchOpenGLContext::CreateOffScreenTexture(int width, int height)
	{
		// Create the default color texture
		glGenTextures(1, &m_ScreenTexture);
		glBindTexture(GL_TEXTURE_2D, m_ScreenTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT,	 NULL);
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

	void TorchOpenGLContext::OnUpdate(uint32_t width, uint32_t height)
	{
		m_GBuffer->OnUpdate(width, height);

		if (m_ScreenFramebuffer)
		{
			glDeleteFramebuffers(1, &m_ScreenFramebuffer);
		}

		if (m_ScreenTexture)
		{
			glDeleteTextures(1, &m_ScreenTexture);
		}

		if (m_SkyboxTextureFramebuffer)
		{
			glDeleteFramebuffers(1, &m_SkyboxTextureFramebuffer);
		}

		if (m_SkyboxTexture)
		{
			glDeleteTextures(1, &m_SkyboxTexture);
		}

		if (m_PosprocessFramebuffer)
		{
			glDeleteFramebuffers(1, &m_PosprocessFramebuffer);
		}

		if (m_OriginalSceneTexture)
		{
			glDeleteTextures(1, &m_OriginalSceneTexture);
		}

		if (m_BrightnessTexture)
		{
			glDeleteTextures(1, &m_BrightnessTexture);
		}

		CreateOffScreenTexture(width, height);
		CreateSkyboxTexture(width, height);
		CreatePostprocessTextures(width, height);

		singleton<PostProcessFactory>().GetPostProcessEffect(PostProcessType::Bloom).OnUpdate(width, height);
		singleton<PostProcessFactory>().GetPostProcessEffect(PostProcessType::SSAO).OnUpdate(width, height);
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

	void TorchOpenGLContext::CreateSkyboxTexture(uint32_t width, uint32_t height)
	{
		// Create the default color texture
		glGenTextures(1, &m_SkyboxTexture);
		glBindTexture(GL_TEXTURE_2D, m_SkyboxTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Create framebuffer
		glGenFramebuffers(1, &m_SkyboxTextureFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_SkyboxTextureFramebuffer);

		// Attach the color texture to the framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SkyboxTexture, 0);

		// Check if the framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void TorchOpenGLContext::CreatePostprocessTextures(uint32_t width, uint32_t height)
	{
		// Create the default color texture
		glGenTextures(1, &m_OriginalSceneTexture);
		glBindTexture(GL_TEXTURE_2D, m_OriginalSceneTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenTextures(1, &m_BrightnessTexture);
		glBindTexture(GL_TEXTURE_2D, m_BrightnessTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Create framebuffer
		glGenFramebuffers(1, &m_PosprocessFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_PosprocessFramebuffer);

		// Attach the color texture to the framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_OriginalSceneTexture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_BrightnessTexture, 0);

		std::vector<uint32_t> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(attachments.size(), attachments.data());

		// Check if the framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void TorchOpenGLContext::BlitFramebuffer(uint32_t src, uint32_t target, int type)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, src);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target); // Blit to default framebuffer
		glBlitFramebuffer(0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, 0, 0, m_WindowPtr->GetWinSpecification().width, m_WindowPtr->GetWinSpecification().height, type, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind framebuffer
	}
	

	void TorchOpenGLContext::DrawFrame()
	{
		//------------------------------------------- renderers & spcifications --------------------------------------------
		auto& atmosphere = m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::Atmosphere)->GetSpecification();
		auto& atmosphereSpcific = std::get<AtmosphericScatteringSpecification>(atmosphere.get());
		auto& cascadeShadowMap = m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::CascadeShadowMap)->GetSpecification();
		auto& shadowMapSpcific = std::get<CascadeShadowMapSpecification>(cascadeShadowMap.get());
		//auto& ssao = m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::SSAO)->GetSpecification();
		//auto& ssaoSpecific = std::get<SSAOSpecification>(ssao.get());
		//----------------------------------------------------------------------------------------------------------------------

		this->UpdateCameraViewport();
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. Geometry pass (render to GBuffer)
		m_GBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_SceneManager->Render();
		m_GBuffer->Unbind();


		// Render Skybox into texture
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, m_SkyboxTextureFramebuffer);
		m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::Atmosphere)->Render();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. Render cascade shadow map
		m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::CascadeShadowMap)->BeginRender();
		SceneManager::GetSceneManager()->RenderScene();
		m_EnvirManager->GetEnvironmentEntityPtr(EnvironmentEntityType::CascadeShadowMap)->EndRender();

		// render ssao
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_EditorCamera->GetViewportWidth(), m_EditorCamera->GetViewportHeight());
		auto& ssaoEffect = singleton<PostProcessFactory>().GetPostProcessEffect(PostProcessType::SSAO);
		auto& ssaoSpeci = std::get<SSAOSpecification>(ssaoEffect.GetSpecification());
		ssaoEffect.BeginRender();
		{
			Texture::BindTexture(0, GL_TEXTURE_2D, m_GBuffer->GetGViewPositionTexture());
			Texture::BindTexture(1, GL_TEXTURE_2D, m_GBuffer->GetGNormalTexture());
			Texture::BindTexture(2, GL_TEXTURE_2D, ssaoSpeci.noiseTexture);
			RenderQuad::Render();
		}
		ssaoEffect.EndRender();
		

		// 3. Lighting pass (render to default framebuffer)
		glBindFramebuffer(GL_FRAMEBUFFER, m_PosprocessFramebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		glViewport(0, 0, m_EditorCamera->GetViewportWidth(), m_EditorCamera->GetViewportHeight());
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

		glBindImageTexture(0, m_GBuffer->GetRedIntTexture(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32I);
		Texture::BindTexture(1, GL_TEXTURE_2D, m_GBuffer->GetGPositionTexture());
		Texture::BindTexture(2, GL_TEXTURE_2D, m_GBuffer->GetGNormalTexture());
		Texture::BindTexture(3, GL_TEXTURE_2D, m_GBuffer->GetGColorTexture());
		Texture::BindTexture(4, GL_TEXTURE_2D, m_GBuffer->GetGRoughnessAOTexture());
		Texture::BindTexture(5, GL_TEXTURE_CUBE_MAP, ibl.GetIrradianceTexture());
		Texture::BindTexture(6, GL_TEXTURE_CUBE_MAP, ibl.GetPrefilterTexture());
		Texture::BindTexture(7, GL_TEXTURE_2D, ibl.GetBrdfLUTTexture());
		Texture::BindTexture(8, GL_TEXTURE_2D_ARRAY, shadowMapSpcific.shadowMapTexture);
		Texture::BindTexture(9, GL_TEXTURE_2D, m_GBuffer->GetGLightSpacePosition());
		Texture::BindTexture(10, GL_TEXTURE_2D, ssaoSpeci.ssaoColorBlurTexture);
		Texture::BindTexture(11, GL_TEXTURE_2D, m_SkyboxTexture);
		Texture::BindTexture(12, GL_TEXTURE_2D, m_GBuffer->GetGDepthTexture());

		RenderQuad::Render();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 4. Blit depth buffer from GBuffer to default framebuffer
		//this->BlitFramebuffer(m_GBuffer->GetFramebufferID(), 0, GL_DEPTH_BUFFER_BIT);
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
			glViewport(0, 0, m_EditorCamera->GetViewportWidth(), m_EditorCamera->GetViewportHeight());

			ibl.RenderIrradianceCubemap();
			ibl.UnbindFramebuffer();
			glViewport(0, 0, m_EditorCamera->GetViewportWidth(), m_EditorCamera->GetViewportHeight());

			ibl.RenderPrefilterCubemap();
			ibl.UnbindFramebuffer();
			glViewport(0, 0, m_EditorCamera->GetViewportWidth(), m_EditorCamera->GetViewportHeight());
		}

		// 8. Copy final rendered result from default framebuffer to texture for ImGui
		//this->BlitFramebuffer(0, m_ScreenFramebuffer, GL_COLOR_BUFFER_BIT);
		auto& bloomEffect = singleton<PostProcessFactory>().GetPostProcessEffect(PostProcessType::Bloom);
		auto& speci = std::get<BloomSpecification>(bloomEffect.GetSpecification());
		speci.m_SrcTexture = m_BrightnessTexture;
		bloomEffect.BeginRender();
		bloomEffect.EndRender();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_EditorCamera->GetViewportWidth(), m_EditorCamera->GetViewportHeight());
		auto& shader = ShaderManager::GetInstance()->GetBloomFinalShaderRef();
		shader.use();
		shader.setInt("scene", 0);
		shader.setInt("bloomBlur", 1);
		Texture::BindTexture(0, GL_TEXTURE_2D, m_OriginalSceneTexture);
		//Texture::BindTexture(1, GL_TEXTURE_2D, bloom.GetBloom().GetMipChain()[0].texture);
		Texture::BindTexture(1, GL_TEXTURE_2D, speci.m_MipTextureChain[0].texture);
		RenderQuad::Render();


		this->BlitFramebuffer(0, m_ScreenFramebuffer, GL_COLOR_BUFFER_BIT);

		

	}

}