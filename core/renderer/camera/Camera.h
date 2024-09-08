#pragma once
#include <pch/pch.h>

namespace core
{
	class Mouse;
	class Keyboard;
	class Camera
	{
	public:
		Camera();
		Camera(const glm::mat4& projection);

		virtual ~Camera();

		const glm::mat4& GetProjection() const;

	protected:
		glm::mat4 m_Projection;
		static Mouse* m_Mouse;
		static Keyboard* m_Keyboard;
	};
}