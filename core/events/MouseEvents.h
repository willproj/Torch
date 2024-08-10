#pragma once
#include "Event.h"

namespace core
{
    template<typename DerivedEvent>
    class MouseButtonEvent : public EventBase<DerivedEvent>
    {
    public:
        MouseButtonEvent(uint32_t button) : m_Button(button) {}

        uint32_t GetButton() const { return m_Button; }

    private:
        uint32_t m_Button;
    };

    class MouseMoveEvent : public EventBase<MouseMoveEvent>
    {
    public:
        static constexpr EventType Type = EventType::MOUSE_MOVE;
        static constexpr std::string_view Name = "Mouse Move";
        static constexpr EventCategory Categories = EventCategory::CATEGORY_MOUSE_FLAG;

        MouseMoveEvent(double xpos, double ypos) : m_CursorPosX(xpos), m_CursorPosY(ypos) {}

        double GetCursorPosX() const { return m_CursorPosX; }
        double GetCursorPosY() const { return m_CursorPosY; }

    private:
        double m_CursorPosX;
        double m_CursorPosY;
    };

    class MousePressEvent : public MouseButtonEvent<MousePressEvent>
    {
    public:
        static constexpr EventType Type = EventType::MOUSE_PRESS;
        static constexpr std::string_view Name = "Mouse Press";
        static constexpr EventCategory Categories = EventCategory::CATEGORY_INPUT_FLAG |
            EventCategory::CATEGORY_MOUSE_BUTTON_FLAG |
            EventCategory::CATEGORY_MOUSE_FLAG;

        MousePressEvent(uint32_t button) : MouseButtonEvent<MousePressEvent>(button) {}
    };

    class MouseReleaseEvent : public MouseButtonEvent<MouseReleaseEvent>
    {
    public:
        static constexpr EventType Type = EventType::MOUSE_RELEASE;
        static constexpr std::string_view Name = "Mouse Release";
        static constexpr EventCategory Categories = EventCategory::CATEGORY_INPUT_FLAG |
            EventCategory::CATEGORY_MOUSE_BUTTON_FLAG |
            EventCategory::CATEGORY_MOUSE_FLAG;

        MouseReleaseEvent(uint32_t button) : MouseButtonEvent<MouseReleaseEvent>(button) {}
    };

    class MouseScrollEvent : public EventBase<MouseScrollEvent>
    {
    public:
        static constexpr EventType Type = EventType::MOUSE_SCROLL;
        static constexpr std::string_view Name = "Mouse Scroll";
        static constexpr EventCategory Categories = EventCategory::CATEGORY_INPUT_FLAG |
            EventCategory::CATEGORY_MOUSE_BUTTON_FLAG |
            EventCategory::CATEGORY_MOUSE_FLAG;

        MouseScrollEvent(double offsetX, double offsetY)
            : m_OffsetX(offsetX), m_OffsetY(offsetY) {}

        double GetOffsetX() const { return m_OffsetX; }
        double GetOffsetY() const { return m_OffsetY; }

    private:
        double m_OffsetX;
        double m_OffsetY;
    };
}
