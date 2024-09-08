#pragma once
#include <pch/pch.h>

namespace core
{
	class Shader
	{
	public:
		Shader() = default;
		Shader(const std::string& computePath);
		Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");

		void InitShader(const std::string& computePath);
		void InitShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");

		void use() const
		{
			glUseProgram(m_ShaderID);
		}

		void setBool(const std::string& name, bool value) const
		{
			glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), (int)value);
		}

		void setInt(const std::string& name, int value) const
		{
			glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), value);
		}

		void setFloat(const std::string& name, float value) const
		{
			glUniform1f(glGetUniformLocation(m_ShaderID, name.c_str()), value);
		}

		void setVec2(const std::string& name, const glm::vec2& value) const
		{
			glUniform2fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, &value[0]);
		}

		void setVec3(const std::string& name, const glm::vec3& value) const
		{
			glUniform3fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, &value[0]);
		}

		void setVec4(const std::string& name, const glm::vec4& value) const
		{
			glUniform4fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, &value[0]);
		}

		void setMat4(const std::string& name, const glm::mat4& mat) const
		{
			glUniformMatrix4fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

	private:
		uint32_t m_ShaderID;

		uint32_t CreateShader(const std::string& source, GLenum shaderType);
		uint32_t CreateShaderProgram(uint32_t vertex, uint32_t fragment, uint32_t geometry = 0);

		void CheckCompileErrors(GLuint shader, std::string type);
		
	};
}
