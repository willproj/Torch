#pragma once
#include <pch/pch.h>
#include <core/platform/Window.h>
#include <core/events/Keyboard.h>
#include <core/events/Mouse.h>
#include <core/graphics/TorchGraphicsContext.h>

namespace utils
{
    class ServiceLocator
    {
    public:
        static void RegisterWindow(std::unique_ptr<core::Window> window);
        static void RegisterKeyboard(std::unique_ptr<core::Keyboard> keyboard);
        static void RegisterMouse(std::unique_ptr<core::Mouse> mouse);
        static void RegisterGraphicsContext(std::unique_ptr<core::TorchGraphicsContext> graphicsContext);
        static core::Window* GetWindow();
        static core::Keyboard* GetKeyboard();
        static core::Mouse* GetMouse();
        static core::TorchGraphicsContext* GetGraphicsContext();

    private:
        static std::optional<std::unique_ptr<core::Window>> s_Window;
        static std::optional<std::unique_ptr<core::Keyboard>> s_Keyboard;
        static std::optional<std::unique_ptr<core::Mouse>> s_Mouse;
        static std::optional<std::unique_ptr<core::TorchGraphicsContext>> s_GraphicsContext;
        static std::mutex s_WindowMutex;
        static std::mutex s_KeyboardMutex;
        static std::mutex s_MouseMutex;
        static std::mutex s_GraphicsContextMutex;
    };
}
