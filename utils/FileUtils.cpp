#include "FileUtils.h"

namespace utils
{
	std::vector<char> FileUtils::ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}


	std::string FileUtils::ReadShaderFile(const std::string& filePath)
	{
		std::ifstream shaderFile;
		std::stringstream shaderStream;

		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			shaderFile.open(filePath.c_str());
			shaderStream << shaderFile.rdbuf();
			shaderFile.close();
		}
		catch (std::ifstream::failure& e)
		{
			TORCH_LOG_ERROR("[{}:{}] ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: {}", __FILE__, __LINE__, e.what());
		}
		return shaderStream.str();
	}
}
