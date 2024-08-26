#pragma once
#include <pch/pch.h>

namespace tinygltf {
    class Model;
}

namespace core
{
	class VertexBuffer;
	class TorchModel
	{
	public:
		TorchModel();
		~TorchModel();

        void RenderModel();
		std::string GetModelPath() const { return m_ModelPath; }

		void LoadModel(const std::string& modelPath);
		void UploadMeshToOpenGL(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, GLuint& vao, GLuint& vbo, GLuint& ebo);
        
	private:
		void ProcessMesh(const tinygltf::Model& model);
		std::vector<GLuint> m_VertexBuffers;
		std::vector<GLuint> m_VertexArrays;
        std::vector<GLuint> m_IndicesCounts;

		std::string m_ModelPath;
	};
}