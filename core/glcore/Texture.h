#pragma once
#include <pch/pch.h>

namespace core
{


	class Texture
	{
	public:
		static uint32_t LoadTexture(const std::string& path);
		static uint32_t GenerateTexture(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type);
	private:

	};
}