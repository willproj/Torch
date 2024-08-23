#pragma once
#include <pch/pch.h>
#include "TorchGraphicsContext.h"
#include "core/glcore/TorchModel.h"
#include "core/glcore/Shader.h"
#include "core/glcore/GBuffer.h"
#include "core/renderer/RenderQuad.h"

namespace core
{
	class TorchOpenGLContext :public TorchGraphicsContext
	{
	public:
		TorchOpenGLContext();
		~TorchOpenGLContext();

		void DrawFrame();
		APIType GetAPIType() const override { return APIType::OpenGL; }
	private:
		TorchModel m_TcModel;
		Shader m_TcShader;
		Shader m_LightingShader;

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

		GBuffer m_GBuffer;
		RenderQuad m_Quad;
	};
}