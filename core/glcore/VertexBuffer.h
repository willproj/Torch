#pragma once

namespace core
{
	class VertexBuffer
	{
	public:
		VertexBuffer();
		~VertexBuffer();

		void AddBufferData(GLsizeiptr size, const void* data);

		void Bind();
		void Unbind();
	private:
		uint32_t m_BufferID;
	};
}