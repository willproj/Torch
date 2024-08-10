#pragma once
#include <pch/pch.h>
#define BIT(x) (1 << x)

namespace core
{
    enum class EventType
    {
        NONE = 0,
        WINDOW_RESIZE,
        WINDOW_CLOSE,
        WINDOW_MOVE,
        MOUSE_MOVE,
        MOUSE_BUTTON,
        MOUSE_PRESS,
        MOUSE_RELEASE,
        MOUSE_SCROLL,
        KEYBOARD,
        KEY_PRESS,
        KEY_RELEASE,
        KEY_REPEAT
    };

    enum class EventCategory : uint32_t
    {
        NONE = 0,
        CATEGORY_INPUT_FLAG = BIT(0),
        CATEGORY_MOUSE_FLAG = BIT(1),
        CATEGORY_MOUSE_BUTTON_FLAG = BIT(2),
        CATEGORY_KEYBOARD_FLAG = BIT(3),
        CATEGORY_WINDOW_FLAG = BIT(4),
    };

    constexpr EventCategory operator|(EventCategory lhs, EventCategory rhs)
    {
        return static_cast<EventCategory>
            (
                static_cast<std::underlying_type_t<EventCategory>>(lhs) |
                static_cast<std::underlying_type_t<EventCategory>>(rhs)
                );
    }

    constexpr EventCategory operator&(EventCategory lhs, EventCategory rhs)
    {
        return static_cast<EventCategory>
            (
                static_cast<std::underlying_type_t<EventCategory>>(lhs) &
                static_cast<std::underlying_type_t<EventCategory>>(rhs)
                );
    }

    constexpr EventCategory operator~(EventCategory category)
    {
        return static_cast<EventCategory>
            (
                ~static_cast<std::underlying_type_t<EventCategory>>(category)
                );
    }

    constexpr bool IsInCategory(EventCategory category, EventCategory check)
    {
        return static_cast<bool>(category & check);
    }

    class Event
    {
    public:
        virtual ~Event() = default;

        virtual EventType GetType() const = 0;
        virtual std::string_view GetName() const = 0;
        virtual EventCategory GetCategories() const = 0;

        bool IsCategory(EventCategory category) const
        {
            return static_cast<bool>(GetCategories() & category);
        }

        bool Handled() const { return m_IsHandled; }
        void SetHandled(bool handled) { m_IsHandled = handled; }

    protected:
        bool m_IsHandled = false;
    };

    template<typename DerivedEvent>
    class EventBase : public Event
    {
    public:
        EventType GetType() const override
        {
            return DerivedEvent::Type;
        }

        std::string_view GetName() const override
        {
            return DerivedEvent::Name;
        }

        EventCategory GetCategories() const override
        {
            return DerivedEvent::Categories;
        }

        static EventType GetStaticType()
        {
            return DerivedEvent::Type;
        }
    };
}
