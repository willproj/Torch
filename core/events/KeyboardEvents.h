#pragma once
#include "Event.h"

namespace core
{
    template<typename DerivedEvent>
    class KeyEventBase : public EventBase<DerivedEvent>
    {
    public:
        KeyEventBase(int keyCode, int scancode, int mods)
            : m_KeyCode(keyCode), m_Scancode(scancode), m_Mods(mods) {}

        int GetKeyCode() const { return m_KeyCode; }
        int GetScancode() const { return m_Scancode; }
        int GetMods() const { return m_Mods; }

    private:
        int m_KeyCode;
        int m_Scancode;
        int m_Mods;
    };

    class KeyPressEvent : public KeyEventBase<KeyPressEvent>
    {
    public:
        static constexpr EventType Type = EventType::KEY_PRESS;
        static constexpr std::string_view Name = "Key Press";
        static constexpr EventCategory Categories = EventCategory::CATEGORY_INPUT_FLAG |
            EventCategory::CATEGORY_KEYBOARD_FLAG;

        KeyPressEvent(int keyCode, int scancode, int mods)
            : KeyEventBase(keyCode, scancode, mods) {}
    };

    class KeyReleaseEvent : public KeyEventBase<KeyReleaseEvent>
    {
    public:
        static constexpr EventType Type = EventType::KEY_RELEASE;
        static constexpr std::string_view Name = "Key Release";
        static constexpr EventCategory Categories = EventCategory::CATEGORY_INPUT_FLAG |
            EventCategory::CATEGORY_KEYBOARD_FLAG;

        KeyReleaseEvent(int keyCode, int scancode, int mods)
            : KeyEventBase(keyCode, scancode, mods) {}
    };

    class KeyRepeatEvent : public KeyEventBase<KeyRepeatEvent>
    {
    public:
        static constexpr EventType Type = EventType::KEY_REPEAT;
        static constexpr std::string_view Name = "Key Repeat";
        static constexpr EventCategory Categories = EventCategory::CATEGORY_INPUT_FLAG |
            EventCategory::CATEGORY_KEYBOARD_FLAG;

        KeyRepeatEvent(int keyCode, int scancode, int mods)
            : KeyEventBase(keyCode, scancode, mods) {}
    };
}
