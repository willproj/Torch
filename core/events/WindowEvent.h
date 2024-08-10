#pragma once
#include "Event.h"

namespace core
{
	class WindowResizeEvent : public EventBase<WindowResizeEvent>
	{
	public:
		static constexpr EventType Type = EventType::WINDOW_RESIZE;
		static constexpr std::string_view Name = "WindowResize";
		static constexpr EventCategory Categories = EventCategory::CATEGORY_WINDOW_FLAG;
		WindowResizeEvent(uint32_t width, uint32_t height)
			:m_Width(width), m_Height(height)
		{
		}
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
	private:
		uint32_t m_Width, m_Height;
	};
}