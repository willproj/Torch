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
		void Initialize();
	private:
		inline static std::shared_ptr<ShaderManager> s_Instance = nullptr;
		ShaderManager() = default;

		Shader m_TestShader;

		// Gbuffer Shaders
		Shader m_GemotryPassShader;
		Shader m_LightingPassShader;

		// Environment Shaders
		Shader m_AtmosphereShader;

		// Lights Shader
		Shader m_LightsIDShader;
		Shader m_LightsShader;

		// Shadow Map Shaders
		Shader m_ShadowMapDepthShader;

	};
}