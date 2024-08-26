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
        static std::unique_ptr<Mouse>& GetInstance();

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

        // Get last mouse position
        double GetLastCursorPosX() const { return m_LastCursorPosX; }
        double GetLastCursorPosY() const { return m_LastCursorPosY; }

        // Get scroll offset
        double GetScrollOffsetY() const { return m_ScrollOffsetY; }

        // Set scroll sensitivity
        void SetScrollSensitivity(double sensitivity) { m_ScrollSensitivity = sensitivity; }

        glm::vec2 GetPositionOffset() const;
        void UpdateMouse();


    private:
        Mouse() = default;

        bool m_LeftButton = false;
        bool m_RightButton = false;

        double m_CursorPosX = 0.0;
        double m_CursorPosY = 0.0;

        double m_LastCursorPosX = 0.0;
        double m_LastCursorPosY = 0.0;

        double m_ScrollOffsetY = 0.0;
        double m_ScrollSensitivity = 0.2; 


        static std::unique_ptr<Mouse> s_Instance;
    };
}
