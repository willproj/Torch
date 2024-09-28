#include "ShaderManager.h"

namespace core
{	
	std::shared_ptr<ShaderManager> ShaderManager::GetInstance()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = std::shared_ptr<ShaderManager>(new ShaderManager());
		}
		return s_Instance;
	}

	void ShaderManager::Initialize()
	{
		std::string root = std::string(PROJECT_ROOT);

		m_TestShader.InitShader(
			root + "/assets/shader/test.vert",
			root + "/assets/shader/test.frag"
		);

		m_LightsShader = Shader(
			root + "/assets/shader/lightShader.vert",
			root + "/assets/shader/lightShader.frag"
		);

		// GBuffer Shader
		m_GemotryPassShader.InitShader(
			root + "/assets/shader/gemotryPass.vert",
			root + "/assets/shader/gemotryPass.frag"
		);

		m_LightingPassShader.InitShader(
			root + "/assets/shader/lightingPass.vert",
			root + "/assets/shader/lightingPass.frag"
		);
		
		// Atmosphere Renderers Shader
		m_AtmosphereShader.InitShader(
			root + "/assets/shader/skybox.vert",
			root + "/assets/shader/skybox.frag"
		);

		// SSAO Shader
		m_SSAOShader.InitShader(
			root + "/assets/shader/ssao.vert",
			root + "/assets/shader/ssao.frag"
		);

		m_SSAOBlurShader.InitShader(
			root + "/assets/shader/ssao.vert",
			root + "/assets/shader/ssaoBlur.frag"
		);


		// Lights Shader
		m_LightsIDShader = Shader(
			root + "/assets/shader/lightShader.vert",
			root + "/assets/shader/lightBufferShader.frag"
		);

		// Shadow Map Depth Shader
		m_ShadowMapDepthShader = Shader(
			root + "/assets/shader/shadowMapDepth.vert",
			root + "/assets/shader/shadowMapDepth.frag",
			root + "/assets/shader/shadowMapDepth.geom"
		);

		// Bloom Shader
		m_BloomUpSampleShader = Shader(
			root + "/assets/shader/upSample.comp"
		);

		m_BloomDownSampleShader = Shader(
			root + "/assets/shader/downSample.comp"
		);

		m_BloomFinalShader = Shader(
			root + "/assets/shader/lightingPass.vert",
			root + "/assets/shader/lightBloomFinal.frag"
		);



	}
}