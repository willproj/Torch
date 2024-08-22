#include "TorchOpenGLContext.h"

namespace core
{
	TorchOpenGLContext::TorchOpenGLContext()
	{
		const std::string filepath = std::string(PROJECT_ROOT) + "/assets/models/sphere/sphere.gltf";
		m_TcModel.LoadModel(filepath);


		m_TcShader = Shader(
			std::string(PROJECT_ROOT)+"/assets/shader/shader.vert",
			std::string(PROJECT_ROOT)+"/assets/shader/shader.frag"
		);

	}

	TorchOpenGLContext::~TorchOpenGLContext()
	{
	}

	void TorchOpenGLContext::DrawFrame()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_TcShader.use();

		// Pass matrices to the shader
		m_TcShader.setMat4("model", modelMatrix);
		m_TcShader.setMat4("view", viewMatrix);
		m_TcShader.setMat4("projection", projectionMatrix);

		m_TcModel.RenderModel();
	}
}