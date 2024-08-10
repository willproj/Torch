#pragma once
#include "MouseEvents.h"

namespace core
{
    enum class MouseButtonType
    {
        LEFT,
        RIGHT,
    };

    class Mouse
    {
    public:
        static Mouse& GetInstance();

        // Prevent copy and assignment
        Mouse(const Mouse&) = delete;
        Mouse& operator=(const Mouse&) = delete;

        // Event handling methods
        void OnEvent(MousePressEvent& event);
        void OnEvent(MouseReleaseEvent& event);
        void OnEvent(MouseMoveEvent& event);
        void OnEvent(MouseScrollEvent& event); // Added to handle scroll

        // Get button states
        bool IsLeftButtonPressed() const { return m_LeftButton; }
        bool IsRightButtonPressed() const { return m_RightButton; }

        // Get mouse position
        double GetCursorPosX() const { return m_CursorPosX; }
        double GetCursorPosY() const { return m_CursorPosY; }

    private:
        Mouse() = default;

        bool m_LeftButton = false;
        bool m_RightButton = false;

        double m_CursorPosX = 0.0;
        double m_CursorPosY = 0.0;

        double m_ScrollOffsetY = 0.0;

        static std::unique_ptr<Mouse> s_Instance;
    };
}
