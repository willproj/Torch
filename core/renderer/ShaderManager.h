#pragma once
#include <pch/pch.h>
#include "core/glcore/Shader.h"

namespace core
{
	class ShaderManager
	{
	public:
		~ShaderManager() = default;
		static std::shared_ptr<ShaderManager> GetInstance();

		Shader& GetTestShaderRef() { return m_TestShader; }
		Shader& GetGemotryPassShaderRef() { return m_GemotryPassShader; }
		Shader& GetLightingPassShaderRef() { return m_LightingPassShader; }
		Shader& GetAtmosphereShaderRef() { return m_AtmosphereShader; }

		Shader& GetLightsIDShader() { return m_LightsIDShader; }
		Shader& GetLightsShader() { return m_LightsShader; }
		Shader& GetShadowMapDepthShader() { return m_ShadowMapDepthShader; }

		Shader& GetSSAOShaderRef() { return m_SSAOShader; }
		Shader& GetSSAOBlurShaderRef() { return m_SSAOBlurShader; }

		Shader& GetBloomUpSampleShaderRef() { return m_BloomUpSampleShader; }
		Shader& GetBloomDownSampleShaderRef() { return m_BloomDownSampleShader; }
		Shader& GetBloomFinalShaderRef() { return m_BloomFinalShader; }

		void Initialize();
	private:
		inline static std::shared_ptr<ShaderManager> s_Instance = nullptr;
		ShaderManager() = default;

		Shader m_TestShader;

		// Gbuffer Shaders
		Shader m_GemotryPassShader;
		Shader m_LightingPassShader;

		// Atmosphere Shaders
		Shader m_AtmosphereShader;
		
		// Shadow Map Shaders
		Shader m_ShadowMapDepthShader;

		// SSAO shader
		Shader m_SSAOShader;
		Shader m_SSAOBlurShader;

		// Lights Shader
		Shader m_LightsIDShader;
		Shader m_LightsShader;

		// Bloom Shader
		Shader m_BloomUpSampleShader;
		Shader m_BloomDownSampleShader;
		Shader m_BloomFinalShader;

		


	};
}