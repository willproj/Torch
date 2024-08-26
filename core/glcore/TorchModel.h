#pragma once
#include <pch/pch.h>

namespace tinygltf
{
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
		std::string GetModelName() const { return m_ModelName; }
		void LoadModel(const std::string &modelPath);

	private:

		void UploadMeshToOpenGL(const std::vector<float> &vertices, const std::vector<unsigned int> &indices, GLuint &vao, GLuint &vbo, GLuint &ebo);
		std::string GetModelNameFromPath(const std::string &path);
		void ProcessMesh(const tinygltf::Model &model);
		std::vector<GLuint> m_VertexBuffers;
		std::vector<GLuint> m_VertexArrays;
		std::vector<GLuint> m_IndicesCounts;

		std::string m_ModelPath;
		std::string m_ModelName;
		
	};
}