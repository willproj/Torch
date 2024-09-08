#include "Camera.h"
#include "core/events/Mouse.h"
#include <utils/ServiceLocator.h>

namespace core
{
	Mouse* Camera::m_Mouse = nullptr;
	Keyboard* Camera::m_Keyboard = nullptr;
	
	Camera::Camera()
		:m_Projection(glm::mat4(1.0f))
	{
		m_Mouse = utils::ServiceLocator::GetMouse();
		m_Keyboard = utils::ServiceLocator::GetKeyboard();
	}

	Camera::Camera(const glm::mat4& projection)
		:m_Projection(projection)
	{
		m_Mouse = utils::ServiceLocator::GetMouse();
		m_Keyboard = utils::ServiceLocator::GetKeyboard();
	}

	Camera::~Camera()
	{

	}

	const glm::mat4& Camera::GetProjection() const
	{
		return m_Projection;
	}
}