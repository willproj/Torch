#pragma once
#include "Event.h"

namespace core
{
	class MouseMoveEvent : public EventBase<MouseMoveEvent>
	{
	public:
		MouseMoveEvent(int xpos, int ypos) : m_CursorPosX(xpos), m_CursorPosY(ypos) {}
	
	private:
		int m_CursorPosX;
		int m_CursorPosY;
	};
}