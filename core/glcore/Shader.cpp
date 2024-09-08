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

    Shader::Shader(const std::string& computePath)
    {
        InitShader(computePath);
    }

    void Shader::InitShader(const std::string& computePath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string computeCode;
        std::ifstream cShaderFile;
        // ensure ifstream objects can throw exceptions:
        cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            cShaderFile.open(computePath);

            std::stringstream cShaderStream;
            // read file's buffer contents into streams
            cShaderStream << cShaderFile.rdbuf();
            // close file handlers
            cShaderFile.close();
            // convert stream into string
            computeCode = cShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* cShaderCode = computeCode.c_str();
        // 2. compile shaders
        unsigned int compute;
        // compute shader
        compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);
        CheckCompileErrors(compute, "COMPUTE");

        // shader Program
        m_ShaderID = glCreateProgram();
        glAttachShader(m_ShaderID, compute);
        glLinkProgram(m_ShaderID);
        CheckCompileErrors(m_ShaderID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(compute);
    }

    void Shader::InitShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
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


    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
    {
        InitShader(vertexPath, fragmentPath, geometryPath);
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
