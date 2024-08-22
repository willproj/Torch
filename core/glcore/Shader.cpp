#include "Shader.h"

namespace core
{
    void Shader::CheckCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                TORCH_LOG_ERROR("[{}:{}] ERROR::SHADER_COMPILATION_ERROR of type: {} \n {} \n -------------", __FILE__, __LINE__, type, infoLog);
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                TORCH_LOG_ERROR("[{}:{}] ERROR::PROGRAM_LINKING_ERROR of type: {} \n {} \n -------------", __FILE__, __LINE__, type, infoLog);
            }
        }
    }


    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
    {
        // Load and compile shaders
        std::string vertexCode = utils::FileUtils::ReadShaderFile(vertexPath);
        std::string fragmentCode = utils::FileUtils::ReadShaderFile(fragmentPath);
        std::string geometryCode;

        if (!geometryPath.empty())  // Check if the geometry path is non-empty
            geometryCode = utils::FileUtils::ReadShaderFile(geometryPath);

        // Create shader objects
        uint32_t vertex = CreateShader(vertexCode, GL_VERTEX_SHADER);
        uint32_t fragment = CreateShader(fragmentCode, GL_FRAGMENT_SHADER);
        uint32_t geometry = 0;

        if (!geometryPath.empty())  // Check again before creating the geometry shader
            geometry = CreateShader(geometryCode, GL_GEOMETRY_SHADER);

        // Link shaders into a program
        m_ShaderID = CreateShaderProgram(vertex, fragment, geometry);

        // Cleanup shaders
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometry)  // Only delete geometry shader if it was created
            glDeleteShader(geometry);
    }


    uint32_t Shader::CreateShader(const std::string& source, GLenum shaderType)
    {
        uint32_t shader = glCreateShader(shaderType);
        const char* sourceStrPtr = source.c_str();
        glShaderSource(shader, 1, &sourceStrPtr, NULL);
        glCompileShader(shader);
        CheckCompileErrors(shader, shaderType == GL_VERTEX_SHADER ? "VERTEX" : shaderType == GL_FRAGMENT_SHADER ? "FRAGMENT" : "GEOMETRY");
        return shader;
    }

    uint32_t Shader::CreateShaderProgram(uint32_t vertex, uint32_t fragment, uint32_t geometry)
    {
        uint32_t program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        if (geometry)
            glAttachShader(program, geometry);
        glLinkProgram(program);
        CheckCompileErrors(program, "PROGRAM");
        return program;
    }

}
