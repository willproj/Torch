#include "TorchOpenGLContext.h"

namespace core
{
	TorchOpenGLContext::TorchOpenGLContext()
	{
		const std::string filepath = std::string(PROJECT_ROOT) + "/assets/models/sphere/sphere.gltf";
		m_TcModel.LoadModel(filepath);

		m_TcShader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/shader.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/shader.frag");

		m_LightingShader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/lighting.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/lighting.frag");

		m_LightingShader.use();
		m_LightingShader.setInt("gPosition", 0);
		m_LightingShader.setInt("gNormal", 1);
		m_LightingShader.setInt("gColorSpec", 2);
	}

	TorchOpenGLContext::~TorchOpenGLContext()
	{
	}

	void TorchOpenGLContext::DrawFrame()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// geometry pass
		m_GBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_TcShader.use();
		// Pass matrices to the shader
		m_TcShader.setMat4("model", modelMatrix);
		m_TcShader.setMat4("view", viewMatrix);
		m_TcShader.setMat4("projection", projectionMatrix);
		m_TcModel.RenderModel();
		m_GBuffer.Unbind();

		// lighting pass
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_LightingShader.use();
		m_GBuffer.BindColorTexture();
		m_GBuffer.BindNormalTexture();
		m_GBuffer.BindPositionTexture();
		glm::vec3 viewpos = glm::vec3(0.0f, 0.0f, -10.0f);
		m_LightingShader.setVec3("viewPos", viewpos);
		m_Quad.renderQuad();
	}
}