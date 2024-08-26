#pragma once
#include <pch/pch.h>

namespace utils
{
	class FileUtils
	{
	public:
		static std::vector<char> ReadFile(const std::string& filename);
		static std::string ReadShaderFile(const std::string& filePath);

		static std::string GetRootPath();
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
		static std::string GetFilenameInFilepath(const std::string& path);

		static std::string OpenFolder();
	};
}