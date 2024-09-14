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
        // Cleanup OpenGL buffers to prevent memory leaks
        for (auto vbo : m_VertexBuffers)
        {
            glDeleteBuffers(1, &vbo);
        }
        for (auto vao : m_VertexArrays)
        {
            glDeleteVertexArrays(1, &vao);
        }
    }

    std::string TorchModel::GetModelNameFromPath(const std::string& path)
    {
        size_t lastSlashPos = path.find_last_of("/\\");
        size_t lastDotPos = path.find_last_of('.');

        if (lastSlashPos != std::string::npos && lastDotPos != std::string::npos && lastDotPos > lastSlashPos)
        {
            return path.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);
        }
        return "";
    }

    void TorchModel::LoadModel(const std::string& modelPath)
    {
        m_ModelPath = modelPath;
        m_ModelName = GetModelNameFromPath(modelPath);
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        std::string err, warn;
        bool ret = false;

        // Determine the file format (ASCII glTF or binary glTF)
        if (modelPath.substr(modelPath.find_last_of(".") + 1) == "gltf")
        {
            ret = loader.LoadASCIIFromFile(&model, &err, &warn, modelPath);  // ASCII-based glTF
        }
        else if (modelPath.substr(modelPath.find_last_of(".") + 1) == "glb")
        {
            ret = loader.LoadBinaryFromFile(&model, &err, &warn, modelPath);  // Binary glTF (glb)
        }
        else
        {
            TORCH_LOG_ERROR("[{}:{}] Error: Unsupported file format.", __FILE__, __LINE__);
            return;
        }

        if (!warn.empty())
        {
            TORCH_LOG_WARN("[{}:{}] Warn: {}", __FILE__, __LINE__, warn);
        }

        if (!err.empty())
        {
            TORCH_LOG_ERROR("[{}:{}] Error: {}", __FILE__, __LINE__, err);
        }

        if (!ret)
        {
            TORCH_LOG_ERROR("[{}:{}] Failed to load the model: {}", __FILE__, __LINE__, ret);
            return;  // Return here to prevent further processing if model loading fails
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

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        GLsizei stride = 8 * sizeof(float); // Assuming normals and UVs are present

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind EBO after VAO
    }

    void TorchModel::RenderModel()
    {
        for (size_t i = 0; i < m_VertexArrays.size(); ++i)
        {
            glBindVertexArray(m_VertexArrays[i]);
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
                // Process indices
                const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
                const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
                const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];

                std::vector<unsigned int> indices;
                switch (indexAccessor.componentType)
                {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    // Directly cast for 32-bit indices
                    indices.assign(reinterpret_cast<const unsigned int*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]),
                        reinterpret_cast<const unsigned int*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset + indexAccessor.count * sizeof(unsigned int)]));
                    break;

                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                {
                    // Convert 16-bit indices to 32-bit
                    const unsigned short* shortIndices = reinterpret_cast<const unsigned short*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
                    indices.resize(indexAccessor.count);
                    for (size_t i = 0; i < indexAccessor.count; ++i)
                    {
                        indices[i] = static_cast<unsigned int>(shortIndices[i]);
                    }
                    break;
                }

                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                {
                    // Convert 8-bit indices to 32-bit
                    const unsigned char* byteIndices = reinterpret_cast<const unsigned char*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
                    indices.resize(indexAccessor.count);
                    for (size_t i = 0; i < indexAccessor.count; ++i)
                    {
                        indices[i] = static_cast<unsigned int>(byteIndices[i]);
                    }
                    break;
                }

                default:
                    TORCH_LOG_ERROR("Unsupported index type. Only unsigned int, unsigned short, and unsigned byte are supported.");
                    continue;  // Skip this primitive if the index type is unsupported
                }

                // Process positions
                const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
                const tinygltf::BufferView& posBufferView = model.bufferViews[posAccessor.bufferView];
                const tinygltf::Buffer& posBuffer = model.buffers[posBufferView.buffer];
                const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);

                // Process normals (optional)
                std::vector<float> normals;
                if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
                {
                    const tinygltf::Accessor& normalAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                    const tinygltf::BufferView& normalBufferView = model.bufferViews[normalAccessor.bufferView];
                    const tinygltf::Buffer& normalBuffer = model.buffers[normalBufferView.buffer];

                    const float* normalData = reinterpret_cast<const float*>(&normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset]);
                    normals.assign(normalData, normalData + normalAccessor.count * 3);
                }

                // Process texture coordinates (optional)
                std::vector<float> texCoords;
                if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
                {
                    const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                    const tinygltf::BufferView& uvBufferView = model.bufferViews[uvAccessor.bufferView];
                    const tinygltf::Buffer& uvBuffer = model.buffers[uvBufferView.buffer];

                    const float* uvData = reinterpret_cast<const float*>(&uvBuffer.data[uvBufferView.byteOffset + uvAccessor.byteOffset]);
                    texCoords.assign(uvData, uvData + uvAccessor.count * 2); // 2 components for UV
                }

                // Interleave vertices, normals, and texcoords into a single buffer
                std::vector<float> interleavedData;
                for (size_t i = 0; i < posAccessor.count; ++i)
                {
                    // Add position (3 components)
                    interleavedData.push_back(positions[i * 3 + 0]);
                    interleavedData.push_back(positions[i * 3 + 1]);
                    interleavedData.push_back(positions[i * 3 + 2]);

                    // Add normal (3 components)
                    if (!normals.empty())
                    {
                        interleavedData.push_back(normals[i * 3 + 0]);
                        interleavedData.push_back(normals[i * 3 + 1]);
                        interleavedData.push_back(normals[i * 3 + 2]);
                    }
                    else
                    {
                        // Add default normal if not present (up direction)
                        interleavedData.push_back(0.0f);
                        interleavedData.push_back(0.0f);
                        interleavedData.push_back(1.0f); // Default normal facing up
                    }

                    // Add texture coordinate (2 components)
                    if (!texCoords.empty())
                    {
                        interleavedData.push_back(texCoords[i * 2 + 0]);
                        interleavedData.push_back(texCoords[i * 2 + 1]);
                    }
                    else
                    {
                        // Add default UVs if not present
                        interleavedData.push_back(0.0f);
                        interleavedData.push_back(0.0f);
                    }
                }

                // Upload data to OpenGL
                GLuint vao = 0, vbo = 0, ebo = 0;
                UploadMeshToOpenGL(interleavedData, indices, vao, vbo, ebo);

                m_VertexBuffers.push_back(vbo);
                m_VertexArrays.push_back(vao);
                m_IndicesCounts.push_back(static_cast<int>(indices.size()));
            }
        }
    }
}
