#pragma once
#include <pch/pch.h>
#include <utils/ServiceLocator.h>
#include "core/glcore/Shader.h"

namespace core
{
	class HDR
	{
	public:
		HDR();
		~HDR();

		void OnUpdate();
		void Bind();
		void Unbind();
		uint32_t GetHDRColorBuffer() { return m_ColorBuffer; }
		uint32_t GetHDRFramebufferID() { return m_HdrFBO; }

		Shader& GetShader() { return m_HDRShader; }
	private:
		float m_Gamma;
		void Create();
		uint32_t m_HdrFBO;
		Window* m_WindowPtr;
		uint32_t m_RboDepth;
		uint32_t m_ColorBuffer;
		Shader m_HDRShader;
	};
}