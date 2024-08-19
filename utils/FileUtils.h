#pragma once
#include <pch/pch.h>

namespace utils
{
	class FileUtils
	{
	public:
		static std::vector<char> ReadFile(const std::string& filename);
	};
}