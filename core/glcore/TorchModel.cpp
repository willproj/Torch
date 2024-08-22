#include "TorchModel.h"
#include "VertexBuffer.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

namespace core
{
	TorchModel::TorchModel()
	{
	}
	TorchModel::~TorchModel()
	{
	}
	void TorchModel::LoadModel(const std::string& filename)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err, warn;

		bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
		if (!warn.empty()) {
			TORCH_LOG_WARN("[{}:{}] Warn: {}", __FILE__, __LINE__, warn);
		}

		if (!err.empty()) {
			TORCH_LOG_ERROR("[{}:{}] Error: {}", __FILE__, __LINE__, err);
		}

		if (!ret) {
			TORCH_LOG_ERROR("[{}:{}] Failed to load the model: {}", __FILE__, __LINE__, ret);
			return;
		}
		TORCH_LOG_INFO("[{}:{}] Success: Load model successfully.", __FILE__, __LINE__);
		ProcessMesh(model);

	}

	void TorchModel::UploadMeshToOpenGL(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, GLuint& vao, GLuint& vbo, GLuint& ebo)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		glBindBuffer (GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	void TorchModel::RenderModel()
	{
		for (size_t i = 0; i < m_VertexBuffers.size(); ++i) {
			glBindVertexArray(m_VertexBuffers[i]);
			glDrawElements(GL_TRIANGLES, m_IndicesCounts[i], GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}

	void TorchModel::ProcessMesh(const tinygltf::Model& model)
	{
		for (const auto& mesh : model.meshes)
		{
			for (const auto& primitive : mesh.primitives)
			{
				const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
				const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
				const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];

				const unsigned char* indexDataPtr = &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset];
				std::vector<unsigned int> indices(reinterpret_cast<const unsigned int*>(indexDataPtr),
					reinterpret_cast<const unsigned int*>(indexDataPtr + indexAccessor.count * sizeof(unsigned int)));

				const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& posBufferView = model.bufferViews[posAccessor.bufferView];
				const tinygltf::Buffer& posBuffer = model.buffers[posBufferView.buffer];

				const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);
				std::vector<float> vertices(positions, positions + posAccessor.count * 3); 

				GLuint vao = 0, vbo = 0, ebo = 0;
				UploadMeshToOpenGL(vertices, indices, vao, vbo, ebo);

				m_VertexBuffers.push_back(vao);
				m_IndicesCounts.push_back(static_cast<GLuint>(indices.size()));
			}
		}
	}
}